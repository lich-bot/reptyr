/*
 * Copyright (C) 2011 by Nelson Elhage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

struct x86_personality {
    size_t ax;
};

struct x86_personality x86_personality[];

static inline struct x86_personality *x86_pers(struct ptrace_child *child) {
    return &x86_personality[child->personality];
}

static inline void arch_fixup_regs(struct ptrace_child *child) {
    struct ptrace_personality *pers = personality(child);
    struct reg *regs = &child->regs;
#define ptr(regs, off) ((unsigned long*)((void*)(regs)+(off)))
    *ptr(regs, pers->reg_ip) -= 2;
}

static inline unsigned long arch_get_register(struct ptrace_child *child, unsigned long oft) {
	struct reg regs;

	(void) ptrace_command(child, PT_GETREGS, &regs);

	return *ptr(&regs,oft);
}

static inline void arch_set_register(struct ptrace_child *child, unsigned long oft, unsigned long val) {
	struct reg regs;

	(void) ptrace_command(child, PT_GETREGS, &regs);
	*ptr(&regs,oft)=val;
	(void) ptrace_command(child, PT_SETREGS, &regs);
}

static inline int arch_get_syscall(struct ptrace_child *child,
                                   unsigned long sysno) {
	return *ptr(&child->regs, personality(child)->syscall_rv);
}

static inline void arch_set_syscall(struct ptrace_child *child,
                                   unsigned long sysno) {
    arch_set_register(child, x86_pers(child)->ax, sysno);
}

#undef ptr
