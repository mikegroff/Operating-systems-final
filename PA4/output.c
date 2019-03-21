#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>
#define DEVICE_NAME "FIFOcharo"
#define CLASS_NAME "FIFO"
#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mike Groff, Matthew Balwant, Angel Mercado");
MODULE_DESCRIPTION("COP4600 - Assignment 4");

static int majorNumber = -1;
extern char message[BUFFER_SIZE];
static int numberOpens = 0;
static struct class* FIFOcharClass = NULL;
static struct device* FIFOcharDevice = NULL;

static ssize_t fifo_read(struct file *file, char *buf, size_t count, loff_t *offset);
static ssize_t fifo_write(struct file *file, const char *buf, size_t count, loff_t *offset);
static int fifo_open(struct inode *inode, struct file *file);
static int fifo_release(struct inode *inode, struct file *file);
static DEFINE_MUTEX(Fifochar_mutex);

static struct file_operations fifo_fops = {
      .open = fifo_open,
      .read = fifo_read,
      .write = fifo_write,
      .release = fifo_release,
};
int FIFOchar_init(void)
{
    mutex_init(&Fifochar_mutex);
    printk(KERN_INFO " %s : Initializing the FIFOchar KM \n", DEVICE_NAME);
    majorNumber = register_chrdev(0, DEVICE_NAME, &fifo_fops);
    if (majorNumber<0)
    {
        printk(KERN_ALERT "FIFOchar failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "%s: registered correctly with major number %d\n", DEVICE_NAME, majorNumber);

    FIFOcharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(FIFOcharClass))
    {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(FIFOcharClass);
    }
    printk(KERN_INFO "%s: device class registered correctly\n", DEVICE_NAME);

    FIFOcharDevice = device_create(FIFOcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(FIFOcharDevice))
    {
        class_destroy(FIFOcharClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(FIFOcharDevice);
    }
    printk(KERN_INFO "%s: device class created correctly\n", DEVICE_NAME); // Made it! device was initialized
    return 0;
}

void FIFOchar_exit(void)
{
   mutex_destroy(&Fifochar_mutex);
   device_destroy(FIFOcharClass, MKDEV(majorNumber, 0));
   class_unregister(FIFOcharClass);
   class_destroy(FIFOcharClass);
   unregister_chrdev(majorNumber, DEVICE_NAME);
   printk(KERN_INFO "%s: Exiting the KM\n", DEVICE_NAME);
}

static int fifo_open(struct inode *inodep, struct file *filep)
{
    if(!mutex_trylock(&Fifochar_mutex)){    /// Try to acquire the mutex (i.e., put the lock on/down)
                                          /// returns 1 if successful and 0 if there is contention
      printk(KERN_ALERT "%s: Device in use by another process", DEVICE_NAME);
      return -EBUSY;
   }
   numberOpens++;
   printk(KERN_INFO "FIFOChar: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}
static int ucfsearch(void)
{
  char key[3] = "UCF";
    int pos_search = 0;
    int pos_text = 0;
    int len_search = 3;
    int len_text = BUFFER_SIZE;
    for (pos_text = 0; pos_text < len_text - len_search;++pos_text)
    {
        if(message[pos_text] == key[pos_search])
        {
            ++pos_search;
            if(pos_search == len_search)
                return pos_text;
        }
        else
        {
           pos_text -=pos_search;
           pos_search = 0;
        }
    }
    return -1;
}
static void editbuff(int postart)
{
  int i = 0;
  char newkey[38] = {'U','n','d','e','f','e','a','t','e','d',' ','2','0','1','8',' ', 'N','a','t','i','o','n','a','l',' ','C','h','a','m','p','i','o','n','s',' ','U','C','F'};
  char oldmes[BUFFER_SIZE];
  strcpy(oldmes,message);
  while(i < postart)
  {
    message[i] = oldmes[i];
    i++;
  }
  while(i >= postart && i < postart+38 )
  {
    message[i]=newkey[i-postart];
    i++;
  }
  while(i < BUFFER_SIZE)
  {
    message[i] = oldmes[i-35];
  }
}

static ssize_t fifo_read(struct file *file, char *buf, size_t count,  loff_t *offset)
{
    if(*offset == 0)
    {
      size_t read = 0;
      while (read < count && (message[read] != 0))
      {
        put_user(message[read], buf++);
        message[read] = 0;
        read++;
        (*offset)++;
      }
      printk(KERN_INFO "%s sending %d chars to the userspace\n", DEVICE_NAME, read > 0 ? read : 0);
      return read;
    }
    else
      return 0;
}

static ssize_t fifo_write(struct file *file, const char *buf, size_t count,loff_t *offset)
{
    size_t pt = 0;int index;
    memset(message, 0, BUFFER_SIZE);
    while(count > 0 && pt < BUFFER_SIZE)
    {
      message[pt] = buf[pt];
      pt++;
      count--;
    }
    index = ucfsearch();
    if(index != -1)
    {
      editbuff(index);
      pt+=35;
    }
    printk(KERN_INFO "%s: wrote %d chars from userspace\n", DEVICE_NAME, pt > 0 ? (pt - 1) : 0);
    return pt;
}

static int fifo_release(struct inode *inodep, struct file *filep)
{
   mutex_unlock(&Fifochar_mutex);
   printk(KERN_INFO "%s: Device successfully closed\n", DEVICE_NAME);
   return 0;
}
