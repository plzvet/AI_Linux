/* led_control.c - Simple LED control character device driver without sysfs class
 *
 * This module registers a character device /dev/led_control via register_chrdev.
 * Writing '1' turns on LED for Player 1, '2' for Player 2.
 * GPIO numbers should be adjusted for your board.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#define DEVICE_NAME "led_control"
#define LED_GPIO1   17  // GPIO pin for Player 1 LED
#define LED_GPIO2   27  // GPIO pin for Player 2 LED

static int major_number;

static ssize_t led_write(struct file *filep, const char __user *buf, size_t count, loff_t *ppos)
{
    char cmd;
    if (count < 1 || copy_from_user(&cmd, buf, 1))
        return -EFAULT;

    // Turn off both LEDs
    gpio_set_value(LED_GPIO1, 0);
    gpio_set_value(LED_GPIO2, 0);

    switch (cmd) {
    case '1':
        gpio_set_value(LED_GPIO1, 1);
        pr_info("led_control: Player1 LED ON\n");
        break;
    case '2':
        gpio_set_value(LED_GPIO2, 1);
        pr_info("led_control: Player2 LED ON\n");
        break;
    default:
        pr_info("led_control: Unknown cmd %c\n", cmd);
        break;
    }
    return count;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = led_write,
};

static int __init led_init(void)
{
    int ret;

    // Request and configure GPIOs
    if (!gpio_is_valid(LED_GPIO1) || !gpio_is_valid(LED_GPIO2)) {
        pr_err("led_control: invalid GPIO pins\n");
        return -ENODEV;
    }
    ret = gpio_request(LED_GPIO1, "LED1");
    if (ret) {
        pr_err("led_control: cannot request GPIO %d\n", LED_GPIO1);
        return ret;
    }
    ret = gpio_request(LED_GPIO2, "LED2");
    if (ret) {
        gpio_free(LED_GPIO1);
        pr_err("led_control: cannot request GPIO %d\n", LED_GPIO2);
        return ret;
    }
    gpio_direction_output(LED_GPIO1, 0);
    gpio_direction_output(LED_GPIO2, 0);

    // Register character device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("led_control: register_chrdev failed\n");
        gpio_free(LED_GPIO1);
        gpio_free(LED_GPIO2);
        return major_number;
    }
    pr_info("led_control: loaded, major=%d. mknod /dev/%s c %d 0\n", major_number, DEVICE_NAME, major_number);
    return 0;
}

static void __exit led_exit(void)
{
    // Cleanup
    gpio_set_value(LED_GPIO1, 0);
    gpio_set_value(LED_GPIO2, 0);
    gpio_free(LED_GPIO1);
    gpio_free(LED_GPIO2);
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("led_control: unloaded\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Simple LED control via write-only char device");
MODULE_VERSION("1.0");
