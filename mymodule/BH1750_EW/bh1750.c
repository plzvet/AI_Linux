#include <linux/module.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/i2c-smbus.h>
#include <linux/delay.h>

#define DEV_NAME "bhdev"
static int major_num;

#define BH1750_ADDR 0x23

#define CONTINUOUSLY_H_RESOLUTION_MODE 0x10

static struct i2c_adapter *adap;
static struct i2c_client *client;

static ssize_t bh1750_read_lux(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    // unsigned char
    __u8 data[2];
    int ret;
    int lux;

    ret = i2c_smbus_write_byte(client,CONTINUOUSLY_H_RESOLUTION_MODE);
    if (ret < 0) return ret;
    msleep(180); // 측정 딜레이 최소 180ms

    ret = i2c_smbus_read_i2c_block_data(client,0x00,2,data);
    if(ret < 0) return ret;
    lux = ((data[0] << 8) | data[1]) * 10 / 12;

	  // 조도센서에서 write한 값을 buf에 복사
    if (copy_to_user(buf, &lux, sizeof(lux)))
        return -EFAULT;
    return sizeof(lux);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = bh1750_read_lux,
};

static int __init bhdev_init(void)
{
    //센서 장치 정보를 담는 구조체 생성
    struct i2c_board_info info = {
        I2C_BOARD_INFO("bh1750", BH1750_ADDR)
    };

    // 1번 i2c버스를 사용
    adap = i2c_get_adapter(1);
    if(!adap) return -ENODEV;

    // 위에서 만든 adap(버스)와 info(센서 정보)를 조합해 새로운 i2c 클라이언트를 생성
    client = i2c_new_client_device(adap,&info);
    if(!client)
    {
        i2c_put_adapter(adap);
        return -ENODEV;
    }

    major_num = register_chrdev(0, DEV_NAME, &fops);
    if (major_num < 0) {
        pr_err("Device registration failed\n");
        return major_num;
    }
    pr_info("Major number: %d\n", major_num);

    return 0;
}

static void __exit bhdev_exit(void)
{
    unregister_chrdev(major_num, DEV_NAME);
    i2c_unregister_device(client);
    i2c_put_adapter(adap);
}


module_init(bhdev_init);
module_exit(bhdev_exit);
MODULE_LICENSE("GPL");