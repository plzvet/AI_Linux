#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>

// BH1750 I2C 명령어
#define BH1750_POWER_ON         0x01
#define BH1750_CONT_H_RES_MODE  0x10

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Park JaeKwon");
MODULE_DESCRIPTION("BH1750 ambient light sensor driver");
//MODULE_SUPPORTED_DEVICE("BH1750 light sensor");

// 드라이버 내부 데이터
struct bh1750_data {
    struct i2c_client  *client;
    struct mutex        lock;
    struct miscdevice   miscdev;
};

// read(): /dev/bh1750에서 호출됨
static ssize_t bh1750_read(struct file *file, char __user *buf,
                           size_t count, loff_t *ppos)
{
    struct bh1750_data *data = file->private_data;
    u8 raw[2];
    int ret, lux, len;
    char str[20];

    mutex_lock(&data->lock);

    // 전원 ON 및 고해상도 모드 시작
    i2c_smbus_write_byte(data->client, BH1750_POWER_ON);
    msleep(10);
    i2c_smbus_write_byte(data->client, BH1750_CONT_H_RES_MODE);
    msleep(180);

    // 센서로부터 2바이트 읽기
    ret = i2c_master_recv(data->client, raw, 2);
    if (ret < 0) {
        mutex_unlock(&data->lock);
        return ret;
    }

    // Lux 계산 (MSB<<8 | LSB) ×10÷12
    lux = (raw[0] << 8) | raw[1];
    lux = lux * 10 / 12;
    len = snprintf(str, sizeof(str), "%d lx\n", lux);

    mutex_unlock(&data->lock);

    // EOF 처리
    if (*ppos)
        return 0;

    // 사용자 공간 복사
    if (copy_to_user(buf, str, len))
        return -EFAULT;

    *ppos = len;
    return len;
}

// open(): file->private_data에 드라이버 데이터 연결
static int bh1750_open(struct inode *inode, struct file *file)
{
    struct miscdevice *misc = file->private_data;
    struct bh1750_data *data = container_of(misc, struct bh1750_data, miscdev);
    file->private_data = data;
    return 0;
}

// 파일 연산 테이블
static const struct file_operations bh1750_fops = {
    .owner = THIS_MODULE,
    .open  = bh1750_open,
    .read  = bh1750_read,
};

// probe(): I2C core이 디바이스 매칭 후 호출
static int bh1750_probe(struct i2c_client *client)
{
    struct bh1750_data *data;
    int ret;

    data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->client = client;
    mutex_init(&data->lock);

    // misc device 설정
    data->miscdev.minor = MISC_DYNAMIC_MINOR;
    data->miscdev.name  = "bh1750";
    data->miscdev.fops  = &bh1750_fops;
    data->miscdev.mode  = 0444;
    data->miscdev.parent = &client->dev;

    ret = misc_register(&data->miscdev);
    if (ret) {
        dev_err(&client->dev, "failed to register /dev/bh1750\n");
        return ret;
    }

    i2c_set_clientdata(client, data);
    dev_info(&client->dev, "BH1750 registered at /dev/bh1750\n");
    return 0;
}

// remove(): 모듈 언로드 시 호출
static void bh1750_remove(struct i2c_client *client)
{
    struct bh1750_data *data = i2c_get_clientdata(client);
    misc_deregister(&data->miscdev);
}

// I2C ID 테이블 (legacy matching용, 필요시)
static const struct i2c_device_id bh1750_id[] = {
    { "bh1750", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, bh1750_id);

// Device Tree 호환 테이블
static const struct of_device_id bh1750_dt_ids[] = {
    { .compatible = "bh,bh1750" },
    { }
};
MODULE_DEVICE_TABLE(of, bh1750_dt_ids);

// i2c_driver 구조체
static struct i2c_driver bh1750_driver = {
    .driver = {
        .name           = "bh1750",
        .of_match_table = bh1750_dt_ids,
    },
    .probe  = bh1750_probe,
    .remove = bh1750_remove,
    .id_table = bh1750_id,
};


module_i2c_driver(bh1750_driver);