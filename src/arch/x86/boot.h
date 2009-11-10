struct gdt_entry
{
  ushort limit_low;
  ushort base_low;
  uchar base_mid;
  uchar access;
  uchar granularity;
  uchar base_high;
}__attribute((packed));

struct gdt_pointer
{
  ushort limit;
  uint base; //Address of first gdt_entry
}__attribute__((packed));

struct idt_entry
{
  ushort base_low;//Lower 16 bits of code to run when interrupt is triggered.
  ushort sel;//Segment selector
  uchar zero;//
  uchar flags;
  ushort base_high;//Higher 16 bits of code to run when interrupt is triggered.
} __attribute__((packed));

struct idt_pointer
{
   ushort limit;
   uint base;//Address of first element in idt_entry array.
} __attribute__((packed));

static void init_dt();

extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void irq0 ();
extern void irq1 ();
extern void irq2 ();
extern void irq3 ();
extern void irq4 ();
extern void irq5 ();
extern void irq6 ();
extern void irq7 ();
extern void irq8 ();
extern void irq9 ();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
