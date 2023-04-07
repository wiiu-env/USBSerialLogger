#include <coreinit/cache.h>
#include <coreinit/debug.h>
#include <coreinit/ios.h>
#include <coreinit/memorymap.h>
#include <coreinit/title.h>
#include <dirent.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sysapp/launch.h>
#include <sysapp/title.h>

extern "C" void SCKernelCopyData(uint32_t dst, uint32_t src, uint32_t len);
extern "C" void KernelCopyDataInternal(uint32_t dst, uint32_t src, uint32_t len);

#define KERN_SYSCALL_TBL1 0xFFE84C70 //Unknown
#define KERN_SYSCALL_TBL2 0xFFE85070 //Games
#define KERN_SYSCALL_TBL3 0xFFE85470 //Loader
#define KERN_SYSCALL_TBL4 0xFFEAAA60 //Home menu
#define KERN_SYSCALL_TBL5 0xFFEAAE60 //Browser

void KernelNOPAtPhysicalAddress(uint32_t addr) {
    uint32_t dst = 0x60000000;
    ICInvalidateRange(&dst, 4);
    DCFlushRange(&dst, 4);
    KernelCopyDataInternal(addr, (uint32_t) OSEffectiveToPhysical((uint32_t) &dst), 4);
}

/* Write a 32-bit word with kernel permissions */
void __attribute__((noinline)) kern_write(void *addr, uint32_t value) {
    asm volatile(
            "li 3,1\n"
            "li 4,0\n"
            "mr 5,%1\n"
            "li 6,0\n"
            "li 7,0\n"
            "lis 8,1\n"
            "mr 9,%0\n"
            "mr %1,1\n"
            "li 0,0x3500\n"
            "sc\n"
            "nop\n"
            "mr 1,%1\n"
            :
            : "r"(addr), "r"(value)
            : "memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
              "11", "12");
}

int main(int argc, char **argv) {
    // Start syslogging on iosu side
    int mcpFd = IOS_Open("/dev/mcp", (IOSOpenMode) 0);
    if (mcpFd >= 0) {
        int in  = 0xFA; // IPC_CUSTOM_START_USB_LOGGING
        int out = 0;
        IOS_Ioctl(mcpFd, 100, &in, sizeof(in), &out, sizeof(out));
        IOS_Close(mcpFd);
    }

    kern_write((void *) (KERN_SYSCALL_TBL1 + 0x25 * 4), (uint32_t) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL2 + 0x25 * 4), (uint32_t) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL3 + 0x25 * 4), (uint32_t) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL4 + 0x25 * 4), (uint32_t) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL5 + 0x25 * 4), (uint32_t) SCKernelCopyData);

    // Patch loader.elf to spit out less warnings when loading .rpx built with wut
    KernelNOPAtPhysicalAddress(0x0100b770 - 0x01000000 + 0x32000000);
    KernelNOPAtPhysicalAddress(0x0100b800 - 0x01000000 + 0x32000000);
    KernelNOPAtPhysicalAddress(0x0100b7b8 - 0x01000000 + 0x32000000);
    ICInvalidateRange(reinterpret_cast<void *>(0x0100b770), 0x04);
    ICInvalidateRange(reinterpret_cast<void *>(0x0100b800), 0x04);
    ICInvalidateRange(reinterpret_cast<void *>(0x0100b7b8), 0x04);

    return 0;
}
