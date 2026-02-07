/*
    asm.h

    This header provides wrappers around inline assembly.
*/

#pragma once

#include "klib/types.h"

#define ASM(c)                  __asm__ volatile (c);

#define HLT                     __asm_hlt();
#define CLI                     __asm_cli();
#define STI                     __asm_sti();
#define OUTB(p, v)              __asm_outb(p, v);
#define OUTW(p, v)              __asm_outw(p, v);
#define OUTL(p, v)              __asm_outl(p, v);
#define INB(p)                  __asm_inb(p);
#define INW(p)                  __asm_inw(p);
#define INL(p)                  __asm_inl(p);
#define INVLPG(m)               __asm_invlpg(m);

// hlt (stops the CPU until an interrupt is received)
static inline void __asm_hlt() {
    __asm__ volatile ("hlt");
}

// cli (cancel interrupts)
static inline void __asm_cli() {
    __asm__ volatile ("cli");
}

// sti (restore interrupts)
static inline void __asm_sti() {
    __asm__ volatile ("sti");
}

// outb (output byte to a port)
static inline void __asm_outb(u16 port, u8 val) {
    __asm__ volatile ("outb %b0, %w1" :: "a"(val), "Nd"(port) : "memory");
}

// outw (output word to a port)
static inline void __asm_outw(u16 port, u16 val) {
    __asm__ volatile ("outw %w0, %w1" :: "a"(val), "Nd"(port) : "memory");
}

// outl (output dword to a port)
static inline void __asm_outl(u16 port, u32 val) {
    __asm__ volatile ("outl %l0, %l1" :: "a"(val), "Nd"(port) : "memory");
}

// inb (input byte in a port)
static inline u8 __asm_inb(u16 port) {
    u8 ret;
    __asm__ volatile ("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

// inw (input word in a port)
static inline u16 __asm_inw(u16 port) {
    u16 ret;
    __asm__ volatile ("inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

// inl (input dword in a port)
static inline u32 __asm_inl(u16 port) {
    u32 ret;
    __asm__ volatile ("inl %w1, %l0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

// invalidate TLB entries
static inline void __asm_invlpg(voidptr m) {
    __asm__ volatile("invlpg (%0)" :: "r"(m) : "memory");
}

// small I/O delay
static inline void iowait(void) {
    OUTB(0x80, 0)
}