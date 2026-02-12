        .arch armv8-a
        .file   "zstd_lazy.c"
        .text
        .global	ZSTD_compressBlock_GREEDY
        .p2align	2
        .type	ZSTD_compressBlock_GREEDY,@function

ZSTD_compressBlock_GREEDY:              
	.cfi_startproc
// %bb.0:
	sub	sp, sp, #208
	.cfi_def_cfa_offset 208
	stp	x29, x30, [sp, #112]            // 16-byte Folded Spill
	stp	x28, x27, [sp, #128]            // 16-byte Folded Spill
	stp	x26, x25, [sp, #144]            // 16-byte Folded Spill
	stp	x24, x23, [sp, #160]            // 16-byte Folded Spill
	stp	x22, x21, [sp, #176]            // 16-byte Folded Spill
	stp	x20, x19, [sp, #192]            // 16-byte Folded Spill
	add	x29, sp, #112
	.cfi_def_cfa w29, 96
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w21, -24
	.cfi_offset w22, -32
	.cfi_offset w23, -40
	.cfi_offset w24, -48
	.cfi_offset w25, -56
	.cfi_offset w26, -64
	.cfi_offset w27, -72
	.cfi_offset w28, -80
	.cfi_offset w30, -88
	.cfi_offset w29, -96
	.cfi_remember_state
	cmp	x4, #32, lsl #12                // =131072
	str	x2, [sp, #48]                   // 8-byte Folded Spill
	b.hi	.LBB3_236
// %bb.1:
	ldr	x24, [x0, #136]
	mov	x23, x4
	mov	x21, x1
	str	x3, [sp, #56]                   // 8-byte Folded Spill
	cbnz	x24, .LBB3_3
// %bb.2:
	mov	x19, x0
	mov	w0, #8                          // =0x8
	movk	w0, #4, lsl #16
	bl	malloc
	mov	x24, x0
	mov	x0, x19
	str	x24, [x19, #136]
.LBB3_3:
	ldr	x19, [sp, #56]                  // 8-byte Folded Reload
	stur	x0, [x29, #-48]                 // 8-byte Folded Spill
	add	x8, x24, #64, lsl #12           // =262144
	mov	w26, #61833                     // =0xf189
	mov	x0, x24
	mov	w1, wzr
	mov	w2, #131072                     // =0x20000
	movk	w26, #44449, lsl #16
	stur	x8, [x29, #-40]                 // 8-byte Folded Spill
	str	x19, [x8]
	bl	memset
	mov	w9, wzr
	cmp	x23, #8
	mov	x8, x19
	b.lo	.LBB3_6
// %bb.4:
	mov	w12, #32768                     // =0x8000
	ldr	x8, [sp, #56]                   // 8-byte Folded Reload
	mov	x9, xzr
	add	x10, x24, #32, lsl #12          // =131072
	mov	w11, #229376                    // =0x38000
	movk	w12, #65532, lsl #16
	mov	w13, #1                         // =0x1
.LBB3_5:                                // =>This Inner Loop Header: Depth=1
	ldp	w14, w15, [x8]
	add	w17, w11, w12
	mul	w14, w14, w26
	and	w15, w15, #0x7
	lsl	w15, w13, w15
	lsr	w14, w14, #17
	add	w15, w17, w15
	ubfiz	x16, x14, #2, #32
	prfm	pstl1keep, [x24, x16]
	ldr	w16, [x24, w14, uxtw #2]
	str	w16, [x10]
	and	w16, w16, #0x1fff
	orr	w15, w15, w16
	str	w15, [x24, w14, uxtw #2]
	ldur	w14, [x8, #1]
	ldur	w15, [x8, #5]
	mul	w14, w14, w26
	lsr	w16, w14, #17
	and	w14, w15, #0x7
	lsl	w17, w13, w14
	add	w14, w12, w11
	ubfiz	x15, x16, #2, #32
	add	w17, w14, w17
	prfm	pstl1keep, [x24, x15]
	ldr	w15, [x24, w16, uxtw #2]
	add	w17, w17, #8, lsl #12           // =32768
	str	w15, [x10, #4]
	and	w15, w15, #0x1fff
	orr	w15, w17, w15
	str	w15, [x24, w16, uxtw #2]
	ldur	w15, [x8, #2]
	ldur	w16, [x8, #6]
	mul	w15, w15, w26
	and	w16, w16, #0x7
	lsl	w16, w13, w16
	lsr	w15, w15, #17
	add	w16, w14, w16
	add	w16, w16, #16, lsl #12          // =65536
	ubfiz	x17, x15, #2, #32
	prfm	pstl1keep, [x24, x17]
	ldr	w17, [x24, w15, uxtw #2]
	str	w17, [x10, #8]
	and	w17, w17, #0x1fff
	orr	w16, w16, w17
	str	w16, [x24, w15, uxtw #2]
	ldur	w15, [x8, #3]
	ldur	w16, [x8, #7]
	mul	w15, w15, w26
	and	w16, w16, #0x7
	lsl	w16, w13, w16
	lsr	w15, w15, #17
	add	w16, w11, w16
	sub	w16, w16, #32, lsl #12          // =131072
	ubfiz	x17, x15, #2, #32
	prfm	pstl1keep, [x24, x17]
	ldr	w17, [x24, w15, uxtw #2]
	str	w17, [x10, #12]
	and	w17, w17, #0x1fff
	orr	w16, w16, w17
	str	w16, [x24, w15, uxtw #2]
	ldr	w15, [x8, #8]!
	ldur	w16, [x8, #-4]
	mul	w16, w16, w26
	and	w15, w15, #0x7
	lsl	w15, w13, w15
	lsr	w16, w16, #17
	add	w14, w14, w15
	add	w14, w14, #32, lsl #12          // =131072
	ubfiz	x17, x16, #2, #32
	prfm	pstl1keep, [x24, x17]
	ldr	w17, [x24, w16, uxtw #2]
	and	w15, w17, #0x1fff
	str	w17, [x10, #16]
	add	x17, x9, #15
	orr	w14, w14, w15
	str	w14, [x24, w16, uxtw #2]
	add	x9, x9, #8
	ldur	w14, [x8, #-3]
	ldur	w15, [x8, #1]
	cmp	x17, x23
	mul	w14, w14, w26
	and	w15, w15, #0x7
	lsl	w15, w13, w15
	lsr	w14, w14, #17
	add	w15, w11, w15
	sub	w15, w15, #16, lsl #12          // =65536
	ubfiz	x16, x14, #2, #32
	prfm	pstl1keep, [x24, x16]
	ldr	w16, [x24, w14, uxtw #2]
	str	w16, [x10, #20]
	and	w16, w16, #0x1fff
	orr	w15, w15, w16
	str	w15, [x24, w14, uxtw #2]
	ldur	w14, [x8, #-2]
	ldur	w15, [x8, #2]
	mul	w14, w14, w26
	and	w15, w15, #0x7
	lsl	w15, w13, w15
	lsr	w14, w14, #17
	add	w15, w11, w15
	sub	w15, w15, #8, lsl #12           // =32768
	ubfiz	x16, x14, #2, #32
	prfm	pstl1keep, [x24, x16]
	ldr	w16, [x24, w14, uxtw #2]
	str	w16, [x10, #24]
	and	w16, w16, #0x1fff
	orr	w15, w15, w16
	str	w15, [x24, w14, uxtw #2]
	ldur	w14, [x8, #-1]
	ldur	w15, [x8, #3]
	mul	w14, w14, w26
	and	w15, w15, #0x7
	lsl	w15, w13, w15
	lsr	w14, w14, #17
	add	w15, w11, w15
	add	w11, w11, #64, lsl #12          // =262144
	ubfiz	x16, x14, #2, #32
	prfm	pstl1keep, [x24, x16]
	ldr	w16, [x24, w14, uxtw #2]
	and	w18, w16, #0x1fff
	str	w16, [x10, #28]
	add	x10, x10, #32
	orr	w15, w15, w18
	str	w15, [x24, w14, uxtw #2]
	b.lo	.LBB3_5
.LBB3_6:
	add	w10, w9, #4
	cmp	w10, w23
	b.hs	.LBB3_9
// %bb.7:
	add	x11, x24, w9, uxtw #2
	mov	w10, w9
	lsl	w9, w9, #15
	add	x10, x10, #4
	mov	w12, #1                         // =0x1
	add	x11, x11, #32, lsl #12          // =131072
.LBB3_8:                                // =>This Inner Loop Header: Depth=1
	ldr	x13, [x8], #1
	add	x10, x10, #1
	cmp	x10, x23
	mul	w14, w13, w26
	ubfx	x13, x13, #32, #3
	lsl	w13, w12, w13
	lsr	w14, w14, #17
	add	w13, w9, w13
	add	w9, w9, #8, lsl #12             // =32768
	ubfiz	x15, x14, #2, #32
	prfm	pstl1keep, [x24, x15]
	ldr	w15, [x24, w14, uxtw #2]
	str	w15, [x11], #4
	and	w15, w15, #0x1fff
	orr	w13, w13, w15
	str	w13, [x24, w14, uxtw #2]
	b.lo	.LBB3_8
.LBB3_9:
	ldr	x8, [sp, #48]                   // 8-byte Folded Reload
	cmp	x23, #10
	ldp	w9, w8, [x8]
	stp	w9, w8, [sp, #40]               // 8-byte Folded Spill
	ldur	x8, [x29, #-48]                 // 8-byte Folded Reload
	ldr	x9, [x8, #8]
	str	x9, [sp, #24]                   // 8-byte Folded Spill
	ldr	w9, [x8, #272]
	str	w9, [sp, #20]                   // 4-byte Folded Spill
	ldr	w9, [x8, #24]
	ldr	w8, [x8, #40]
	stp	w8, w9, [sp, #32]               // 8-byte Folded Spill
	ldr	x8, [sp, #56]                   // 8-byte Folded Reload
	add	x10, x8, x23
	add	x30, x8, #1
	str	x30, [sp, #8]                   // 8-byte Folded Spill
	b.lt	.LBB3_234
// %bb.10:
	mov	w15, wzr
	mov	w26, wzr
	sub	x17, x10, #8
	sub	x18, x10, #7
	sub	x11, x10, #3
	sub	x9, x10, #1
	add	x2, x24, #32, lsl #12           // =131072
	sub	x0, x10, #32
	mov	w1, #1                          // =0x1
	mov	x23, x8
	stp	x9, x11, [x29, #-16]            // 16-byte Folded Spill
	stur	x0, [x29, #-32]                 // 8-byte Folded Spill
	b	.LBB3_13
.LBB3_11:                               //   in Loop: Header=BB3_13 Depth=1
	ldur	x9, [x29, #-48]                 // 8-byte Folded Reload
	sub	x8, x30, x23
	ldur	x0, [x29, #-32]                 // 8-byte Folded Reload
	lsr	x8, x8, #5
	ldr	w9, [x9, #144]
	add	w8, w9, w8
	add	x30, x30, w8, sxtw
.LBB3_12:                               //   in Loop: Header=BB3_13 Depth=1
	cmp	x30, x17
	b.hs	.LBB3_235
.LBB3_13:                               // =>This Loop Header: Depth=1
                                        //     Child Loop BB3_18 Depth 2
                                        //     Child Loop BB3_49 Depth 2
                                        //       Child Loop BB3_54 Depth 3
                                        //     Child Loop BB3_75 Depth 2
                                        //     Child Loop BB3_107 Depth 2
                                        //       Child Loop BB3_114 Depth 3
                                        //       Child Loop BB3_138 Depth 3
                                        //         Child Loop BB3_143 Depth 4
                                        //       Child Loop BB3_164 Depth 3
                                        //     Child Loop BB3_190 Depth 2
                                        //     Child Loop BB3_199 Depth 2
                                        //     Child Loop BB3_36 Depth 2
                                        //     Child Loop BB3_93 Depth 2
                                        //     Child Loop BB3_97 Depth 2
                                        //     Child Loop BB3_100 Depth 2
                                        //     Child Loop BB3_205 Depth 2
                                        //       Child Loop BB3_209 Depth 3
	mov	x14, x30
	mov	w9, w15
	ldr	w8, [x14, #1]!
	neg	x11, x9
	stur	x11, [x29, #-24]                // 8-byte Folded Spill
	cbz	w15, .LBB3_21
// %bb.14:                              //   in Loop: Header=BB3_13 Depth=1
	sub	x9, x14, x9
	mov	w27, #1                         // =0x1
	ldr	w9, [x9]
	cmp	w9, w8
	b.ne	.LBB3_44
// %bb.15:                              //   in Loop: Header=BB3_13 Depth=1
	ldur	x16, [x29, #-24]                // 8-byte Folded Reload
	add	x9, x30, #5
	cmp	x9, x18
	add	x12, x9, x16
	b.hs	.LBB3_22
// %bb.16:                              //   in Loop: Header=BB3_13 Depth=1
	ldr	x11, [x12]
	ldr	x12, [x9]
	cmp	x11, x12
	b.ne	.LBB3_43
// %bb.17:                              //   in Loop: Header=BB3_13 Depth=1
	add	x11, x30, #13
	mov	x13, xzr
	add	x12, x11, x16
.LBB3_18:                               //   Parent Loop BB3_13 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	cmp	x11, x18
	b.hs	.LBB3_23
// %bb.19:                              //   in Loop: Header=BB3_18 Depth=2
	ldr	x16, [x12], #8
	add	x13, x13, #8
	ldr	x3, [x11], #8
	cmp	x16, x3
	b.eq	.LBB3_18
// %bb.20:                              //   in Loop: Header=BB3_13 Depth=1
	eor	x9, x3, x16
	rbit	x9, x9
	clz	x9, x9
	add	x9, x13, x9, lsr #3
	add	x27, x9, #4
	cmp	x27, #10
	b.hi	.LBB3_32
	b	.LBB3_44
.LBB3_21:                               //   in Loop: Header=BB3_13 Depth=1
	mov	w27, #1                         // =0x1
	b	.LBB3_44
.LBB3_22:                               //   in Loop: Header=BB3_13 Depth=1
	mov	x11, x9
.LBB3_23:                               //   in Loop: Header=BB3_13 Depth=1
	ldur	x13, [x29, #-8]                 // 8-byte Folded Reload
	cmp	x11, x13
	b.hs	.LBB3_26
// %bb.24:                              //   in Loop: Header=BB3_13 Depth=1
	ldr	w13, [x12]
	ldr	w16, [x11]
	cmp	w13, w16
	b.ne	.LBB3_26
// %bb.25:                              //   in Loop: Header=BB3_13 Depth=1
	add	x11, x11, #4
	add	x12, x12, #4
.LBB3_26:                               //   in Loop: Header=BB3_13 Depth=1
	ldur	x13, [x29, #-16]                // 8-byte Folded Reload
	cmp	x11, x13
	b.hs	.LBB3_29
// %bb.27:                              //   in Loop: Header=BB3_13 Depth=1
	ldrh	w13, [x12]
	ldrh	w16, [x11]
	cmp	w13, w16
	b.ne	.LBB3_29
// %bb.28:                              //   in Loop: Header=BB3_13 Depth=1
	add	x11, x11, #2
	add	x12, x12, #2
.LBB3_29:                               //   in Loop: Header=BB3_13 Depth=1
	cmp	x11, x10
	b.hs	.LBB3_31
// %bb.30:                              //   in Loop: Header=BB3_13 Depth=1
	mov	x13, x11
	ldrb	w12, [x12]
	ldrb	w16, [x13], #1
	cmp	w12, w16
	csel	x11, x11, x13, ne
.LBB3_31:                               //   in Loop: Header=BB3_13 Depth=1
	sub	x9, x11, x9
	add	x27, x9, #4
	cmp	x27, #10
	b.ls	.LBB3_44
.LBB3_32:                               //   in Loop: Header=BB3_13 Depth=1
	mov	w12, #1                         // =0x1
	ldr	x9, [x21, #24]
	cmp	x14, x0
	sub	x8, x14, x23
	b.ls	.LBB3_196
.LBB3_33:                               //   in Loop: Header=BB3_13 Depth=1
	cmp	x23, x0
	b.hi	.LBB3_38
// %bb.34:                              //   in Loop: Header=BB3_13 Depth=1
	sub	x11, x0, x23
	ldr	q0, [x23]
	cmp	x11, #17
	add	x11, x9, x11
	str	q0, [x9]
	b.lt	.LBB3_37
// %bb.35:                              //   in Loop: Header=BB3_13 Depth=1
	add	x9, x9, #16
	add	x13, x23, #32
.LBB3_36:                               //   Parent Loop BB3_13 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	ldur	q0, [x13, #-16]
	str	q0, [x9]
	ldr	q0, [x13], #32
	str	q0, [x9, #16]
	add	x9, x9, #32
	cmp	x9, x11
	b.lo	.LBB3_36
.LBB3_37:                               //   in Loop: Header=BB3_13 Depth=1
	mov	x23, x0
	mov	x9, x11
.LBB3_38:                               //   in Loop: Header=BB3_13 Depth=1
	cmp	x23, x14
	b.hs	.LBB3_200
// %bb.39:                              //   in Loop: Header=BB3_13 Depth=1
	sub	x11, x14, x23
	cmp	x11, #8
	b.lo	.LBB3_79
// %bb.40:                              //   in Loop: Header=BB3_13 Depth=1
	sub	x13, x9, x23
	cmp	x13, #32
	b.lo	.LBB3_79
// %bb.41:                              //   in Loop: Header=BB3_13 Depth=1
	cmp	x11, #32
	b.hs	.LBB3_92
// %bb.42:                              //   in Loop: Header=BB3_13 Depth=1
	mov	x3, xzr
	b	.LBB3_96
.LBB3_43:                               //   in Loop: Header=BB3_13 Depth=1
	eor	x9, x12, x11
	rbit	x9, x9
	clz	x9, x9
	lsr	x9, x9, #3
	add	x27, x9, #4
	cmp	x27, #10
	b.hi	.LBB3_32
.LBB3_44:                               //   in Loop: Header=BB3_13 Depth=1
	ldur	x9, [x29, #-40]                 // 8-byte Folded Reload
	ubfx	w8, w8, #24, #3
	ldr	x5, [x9]
	sub	x11, x30, x5
	ldr	w13, [x2, w11, uxtw #2]
	lsr	w9, w13, w8
	sub	w8, w27, #1
	tbz	w9, #0, .LBB3_101
// %bb.45:                              //   in Loop: Header=BB3_13 Depth=1
	cmp	w8, #3
	mov	w12, #3                         // =0x3
	mov	x9, xzr
	csel	w12, w8, w12, hi
	mov	w7, #4                          // =0x4
	sub	w8, w12, #3
	add	w11, w8, w11
	ldr	w16, [x2, w11, uxtw #2]
	cmp	w16, w13
	csel	w11, w8, wzr, lo
	add	x8, x30, w11, sxtw
	ldrb	w8, [x8, #4]
	and	w3, w8, #0x7
	add	x8, x30, #4
	lsl	w19, w1, w3
	csel	w3, w16, w13, lo
	sub	x16, x30, #3
	b	.LBB3_49
.LBB3_46:                               //   in Loop: Header=BB3_49 Depth=2
	mov	x9, x13
.LBB3_47:                               //   in Loop: Header=BB3_49 Depth=2
	mov	w12, w3
.LBB3_48:                               //   in Loop: Header=BB3_49 Depth=2
	subs	w7, w7, #1
	mov	w3, w22
	b.eq	.LBB3_90
.LBB3_49:                               //   Parent Loop BB3_13 Depth=1
                                        // =>  This Loop Header: Depth=2
                                        //       Child Loop BB3_54 Depth 3
	lsr	w13, w3, #15
	subs	w20, w13, w11
	b.ls	.LBB3_90
// %bb.50:                              //   in Loop: Header=BB3_49 Depth=2
	add	x4, x5, w13, uxtw
	ldr	w22, [x2, w13, uxtw #2]
	sub	x13, x4, w11, uxtw
	tst	w22, w19
	b.eq	.LBB3_73
// %bb.51:                              //   in Loop: Header=BB3_49 Depth=2
	mov	w3, w12
	add	x4, x13, x3
	ldr	w3, [x16, x3]
	ldur	w4, [x4, #-3]
	cmp	w4, w3
	b.ne	.LBB3_48
// %bb.52:                              //   in Loop: Header=BB3_49 Depth=2
	ldr	x3, [x13]
	ldr	x4, [x30]
	cmp	x3, x4
	b.ne	.LBB3_57
// %bb.53:                              //   in Loop: Header=BB3_49 Depth=2
	mov	x24, xzr
.LBB3_54:                               //   Parent Loop BB3_13 Depth=1
                                        //     Parent Loop BB3_49 Depth=2
                                        // =>    This Inner Loop Header: Depth=3
	add	x3, x30, x24
	add	x6, x3, #8
	add	x3, x13, x24
	cmp	x6, x18
	b.hs	.LBB3_60
// %bb.55:                              //   in Loop: Header=BB3_54 Depth=3
	ldr	x3, [x3, #8]
	ldr	x4, [x6]
	add	x24, x24, #8
	cmp	x3, x4
	b.eq	.LBB3_54
// %bb.56:                              //   in Loop: Header=BB3_49 Depth=2
	eor	x3, x4, x3
	rbit	x3, x3
	clz	x3, x3
	add	x3, x24, x3, lsr #3
	cmp	w12, w3
	b.lo	.LBB3_58
	b	.LBB3_69
.LBB3_57:                               //   in Loop: Header=BB3_49 Depth=2
	eor	x3, x4, x3
	rbit	x3, x3
	clz	x3, x3
	lsr	x3, x3, #3
	cmp	w12, w3
	b.hs	.LBB3_69
.LBB3_58:                               //   in Loop: Header=BB3_49 Depth=2
	add	w9, w20, w3
	sub	w9, w9, #4
	ldr	w9, [x2, w9, uxtw #2]
	cmp	w22, w9
	b.ls	.LBB3_46
// %bb.59:                              //   in Loop: Header=BB3_49 Depth=2
	sub	w11, w3, #4
	mov	w22, w9
	mov	x9, x13
	b	.LBB3_72
.LBB3_60:                               //   in Loop: Header=BB3_49 Depth=2
	ldur	x0, [x29, #-8]                  // 8-byte Folded Reload
	add	x3, x3, #8
	cmp	x6, x0
	b.hs	.LBB3_63
// %bb.61:                              //   in Loop: Header=BB3_49 Depth=2
	ldr	w4, [x3]
	ldr	w25, [x6]
	cmp	w4, w25
	b.ne	.LBB3_63
// %bb.62:                              //   in Loop: Header=BB3_49 Depth=2
	add	x3, x30, x24
	add	x4, x13, x24
	add	x6, x3, #12
	add	x3, x4, #12
.LBB3_63:                               //   in Loop: Header=BB3_49 Depth=2
	ldur	x0, [x29, #-16]                 // 8-byte Folded Reload
	cmp	x6, x0
	b.hs	.LBB3_66
// %bb.64:                              //   in Loop: Header=BB3_49 Depth=2
	ldrh	w4, [x3]
	ldrh	w24, [x6]
	cmp	w4, w24
	b.ne	.LBB3_66
// %bb.65:                              //   in Loop: Header=BB3_49 Depth=2
	add	x6, x6, #2
	add	x3, x3, #2
.LBB3_66:                               //   in Loop: Header=BB3_49 Depth=2
	cmp	x6, x10
	b.hs	.LBB3_68
// %bb.67:                              //   in Loop: Header=BB3_49 Depth=2
	mov	x4, x6
	ldrb	w3, [x3]
	ldrb	w24, [x4], #1
	cmp	w3, w24
	csel	x6, x6, x4, ne
.LBB3_68:                               //   in Loop: Header=BB3_49 Depth=2
	sub	x3, x6, x30
	cmp	w12, w3
	b.lo	.LBB3_58
.LBB3_69:                               //   in Loop: Header=BB3_49 Depth=2
	sub	w13, w12, #3
	cmp	w11, w13
	b.eq	.LBB3_48
// %bb.70:                              //   in Loop: Header=BB3_49 Depth=2
	add	w3, w20, w12
	sub	w3, w3, #3
	ldr	w3, [x2, w3, uxtw #2]
	cmp	w22, w3
	b.ls	.LBB3_48
// %bb.71:                              //   in Loop: Header=BB3_49 Depth=2
	mov	w11, w13
	mov	w22, w3
	mov	w3, w12
.LBB3_72:                               //   in Loop: Header=BB3_49 Depth=2
	ldrb	w12, [x8, w11, sxtw]
	and	w12, w12, #0x7
	lsl	w19, w1, w12
	b	.LBB3_47
.LBB3_73:                               //   in Loop: Header=BB3_13 Depth=1
	ldr	x8, [x13]
	ldr	x16, [x30]
	cmp	x8, x16
	b.ne	.LBB3_78
// %bb.74:                              //   in Loop: Header=BB3_13 Depth=1
	sub	x6, x5, w11, uxtw
	mov	w3, w3
	mov	x16, xzr
	add	x11, x6, x3, lsr #15
.LBB3_75:                               //   Parent Loop BB3_13 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	add	x8, x30, x16
	add	x8, x8, #8
	cmp	x8, x18
	b.hs	.LBB3_80
// %bb.76:                              //   in Loop: Header=BB3_75 Depth=2
	add	x4, x11, x16
	ldr	x8, [x8]
	add	x16, x16, #8
	ldr	x4, [x4, #8]
	cmp	x4, x8
	b.eq	.LBB3_75
// %bb.77:                              //   in Loop: Header=BB3_13 Depth=1
	eor	x8, x8, x4
	rbit	x8, x8
	clz	x8, x8
	add	x8, x16, x8, lsr #3
	b	.LBB3_89
.LBB3_78:                               //   in Loop: Header=BB3_13 Depth=1
	eor	x8, x16, x8
	rbit	x8, x8
	clz	x8, x8
	lsr	x8, x8, #3
	b	.LBB3_89
.LBB3_79:                               //   in Loop: Header=BB3_13 Depth=1
	mov	x13, x9
	mov	x16, x23
	b	.LBB3_100
.LBB3_80:                               //   in Loop: Header=BB3_13 Depth=1
	add	x11, x6, x3, lsr #15
	ldur	x0, [x29, #-8]                  // 8-byte Folded Reload
	add	x11, x11, x16
	cmp	x8, x0
	add	x11, x11, #8
	b.hs	.LBB3_83
// %bb.81:                              //   in Loop: Header=BB3_13 Depth=1
	ldr	w4, [x11]
	ldr	w7, [x8]
	cmp	w4, w7
	b.ne	.LBB3_83
// %bb.82:                              //   in Loop: Header=BB3_13 Depth=1
	add	x8, x6, x3, lsr #15
	add	x11, x30, x16
	add	x16, x8, x16
	add	x8, x11, #12
	add	x11, x16, #12
.LBB3_83:                               //   in Loop: Header=BB3_13 Depth=1
	ldur	x16, [x29, #-16]                // 8-byte Folded Reload
	cmp	x8, x16
	b.hs	.LBB3_86
// %bb.84:                              //   in Loop: Header=BB3_13 Depth=1
	ldrh	w16, [x11]
	ldrh	w3, [x8]
	cmp	w16, w3
	b.ne	.LBB3_86
// %bb.85:                              //   in Loop: Header=BB3_13 Depth=1
	add	x8, x8, #2
	add	x11, x11, #2
.LBB3_86:                               //   in Loop: Header=BB3_13 Depth=1
	cmp	x8, x10
	b.hs	.LBB3_88
// %bb.87:                              //   in Loop: Header=BB3_13 Depth=1
	mov	x16, x8
	ldrb	w11, [x11]
	ldrb	w3, [x16], #1
	cmp	w11, w3
	csel	x8, x8, x16, ne
.LBB3_88:                               //   in Loop: Header=BB3_13 Depth=1
	sub	x8, x8, x30
.LBB3_89:                               //   in Loop: Header=BB3_13 Depth=1
	cmp	w12, w8
	csel	x9, x13, x9, lo
	csel	w12, w12, w8, hi
.LBB3_90:                               //   in Loop: Header=BB3_13 Depth=1
	cbz	x9, .LBB3_104
// %bb.91:                              //   in Loop: Header=BB3_13 Depth=1
	sub	x8, x30, x9
	mov	w11, w12
	add	x8, x8, #3
	b	.LBB3_105
.LBB3_92:                               //   in Loop: Header=BB3_13 Depth=1
	and	x3, x11, #0xffffffffffffffe0
	add	x13, x23, #16
	add	x16, x9, #16
	mov	x4, x3
.LBB3_93:                               //   Parent Loop BB3_13 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	ldp	q0, q1, [x13, #-16]
	subs	x4, x4, #32
	add	x13, x13, #32
	stp	q0, q1, [x16, #-16]
	add	x16, x16, #32
	b.ne	.LBB3_93
// %bb.94:                              //   in Loop: Header=BB3_13 Depth=1
	cmp	x11, x3
	b.eq	.LBB3_200
// %bb.95:                              //   in Loop: Header=BB3_13 Depth=1
	tst	x11, #0x18
	b.eq	.LBB3_99
.LBB3_96:                               //   in Loop: Header=BB3_13 Depth=1
	and	x4, x11, #0xfffffffffffffff8
	add	x5, x23, x3
	add	x13, x9, x4
	add	x16, x23, x4
	add	x9, x9, x3
	sub	x3, x3, x4
.LBB3_97:                               //   Parent Loop BB3_13 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	ldr	d0, [x5], #8
	adds	x3, x3, #8
	str	d0, [x9], #8
	b.ne	.LBB3_97
// %bb.98:                              //   in Loop: Header=BB3_13 Depth=1
	ldur	x0, [x29, #-32]                 // 8-byte Folded Reload
	cmp	x11, x4
	b.ne	.LBB3_100
	b	.LBB3_200
.LBB3_99:                               //   in Loop: Header=BB3_13 Depth=1
	add	x13, x9, x3
	add	x16, x23, x3
.LBB3_100:                              //   Parent Loop BB3_13 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	ldrb	w9, [x16], #1
	cmp	x16, x14
	strb	w9, [x13], #1
	b.ne	.LBB3_100
	b	.LBB3_200
.LBB3_101:                              //   in Loop: Header=BB3_13 Depth=1
	cmp	w8, #3
	b.hi	.LBB3_104
// %bb.102:                             //   in Loop: Header=BB3_13 Depth=1
	lsr	x8, x13, #15
	ldr	w9, [x30]
	ldr	w12, [x5, x8]
	cmp	w12, w9
	b.ne	.LBB3_104
// %bb.103:                             //   in Loop: Header=BB3_13 Depth=1
	sub	w8, w11, w8
	mov	w11, #4                         // =0x4
	add	w8, w8, #3
	b	.LBB3_105
.LBB3_104:                              //   in Loop: Header=BB3_13 Depth=1
	mov	w8, #51711                      // =0xc9ff
	mov	x11, xzr
	movk	w8, #15258, lsl #16
.LBB3_105:                              //   in Loop: Header=BB3_13 Depth=1
	cmp	x11, x27
	csel	x9, x11, x27, hi
	cmp	x9, #4
	b.lo	.LBB3_11
// %bb.106:                             //   in Loop: Header=BB3_13 Depth=1
	cmp	x11, x27
	ldur	x11, [x29, #-24]                // 8-byte Folded Reload
	csinc	x8, x8, xzr, hi
	add	x12, x30, x11
	csel	x11, x30, x14, hi
	add	x13, x12, #13
.LBB3_107:                              //   Parent Loop BB3_13 Depth=1
                                        // =>  This Loop Header: Depth=2
                                        //       Child Loop BB3_114 Depth 3
                                        //       Child Loop BB3_138 Depth 3
                                        //         Child Loop BB3_143 Depth 4
                                        //       Child Loop BB3_164 Depth 3
	cmp	x30, x17
	b.hs	.LBB3_185
// %bb.108:                             //   in Loop: Header=BB3_107 Depth=2
	add	x24, x30, #1
	cbz	x8, .LBB3_117
// %bb.109:                             //   in Loop: Header=BB3_107 Depth=2
	cbz	w15, .LBB3_131
// %bb.110:                             //   in Loop: Header=BB3_107 Depth=2
	ldur	x16, [x29, #-24]                // 8-byte Folded Reload
	ldr	w12, [x24]
	ldr	w14, [x24, x16]
	cmp	w12, w14
	b.ne	.LBB3_131
// %bb.111:                             //   in Loop: Header=BB3_107 Depth=2
	add	x12, x30, #5
	cmp	x12, x18
	add	x14, x12, x16
	b.hs	.LBB3_118
// %bb.112:                             //   in Loop: Header=BB3_107 Depth=2
	ldr	x14, [x14]
	ldr	x16, [x12]
	cmp	x14, x16
	b.ne	.LBB3_130
// %bb.113:                             //   in Loop: Header=BB3_107 Depth=2
	mov	w3, #13                         // =0xd
	mov	x14, x13
.LBB3_114:                              //   Parent Loop BB3_13 Depth=1
                                        //     Parent Loop BB3_107 Depth=2
                                        // =>    This Inner Loop Header: Depth=3
	add	x16, x30, x3
	cmp	x16, x18
	b.hs	.LBB3_119
// %bb.115:                             //   in Loop: Header=BB3_114 Depth=3
	ldr	x4, [x14], #8
	ldr	x16, [x16]
	add	x3, x3, #8
	cmp	x4, x16
	b.eq	.LBB3_114
// %bb.116:                             //   in Loop: Header=BB3_107 Depth=2
	eor	x12, x16, x4
	rbit	x12, x12
	clz	x12, x12
	add	x12, x3, x12, lsr #3
	sub	x12, x12, #13
	mov	w14, #65531                     // =0xfffb
	movk	w14, #32767, lsl #16
	cmp	w12, w14
	b.ls	.LBB3_128
	b	.LBB3_131
.LBB3_117:                              //   in Loop: Header=BB3_107 Depth=2
	mov	x12, xzr
	mov	x14, x11
	b	.LBB3_132
.LBB3_118:                              //   in Loop: Header=BB3_107 Depth=2
	mov	x16, x12
.LBB3_119:                              //   in Loop: Header=BB3_107 Depth=2
	ldur	x0, [x29, #-8]                  // 8-byte Folded Reload
	cmp	x16, x0
	b.hs	.LBB3_122
// %bb.120:                             //   in Loop: Header=BB3_107 Depth=2
	ldr	w3, [x14]
	ldr	w4, [x16]
	cmp	w3, w4
	b.ne	.LBB3_122
// %bb.121:                             //   in Loop: Header=BB3_107 Depth=2
	add	x16, x16, #4
	add	x14, x14, #4
.LBB3_122:                              //   in Loop: Header=BB3_107 Depth=2
	ldur	x0, [x29, #-16]                 // 8-byte Folded Reload
	cmp	x16, x0
	b.hs	.LBB3_125
// %bb.123:                             //   in Loop: Header=BB3_107 Depth=2
	ldrh	w3, [x14]
	ldrh	w4, [x16]
	cmp	w3, w4
	b.ne	.LBB3_125
// %bb.124:                             //   in Loop: Header=BB3_107 Depth=2
	add	x16, x16, #2
	add	x14, x14, #2
.LBB3_125:                              //   in Loop: Header=BB3_107 Depth=2
	cmp	x16, x10
	b.hs	.LBB3_127
// %bb.126:                             //   in Loop: Header=BB3_107 Depth=2
	mov	x3, x16
	ldrb	w14, [x14]
	ldrb	w4, [x3], #1
	cmp	w14, w4
	csel	x16, x16, x3, ne
.LBB3_127:                              //   in Loop: Header=BB3_107 Depth=2
	sub	x12, x16, x12
	mov	w14, #65531                     // =0xfffb
	movk	w14, #32767, lsl #16
	cmp	w12, w14
	b.hi	.LBB3_131
.LBB3_128:                              //   in Loop: Header=BB3_107 Depth=2
	add	w14, w8, #1
	add	w3, w9, w9, lsl #1
	add	w16, w12, #4
	clz	w12, w14
	add	w14, w16, w16, lsl #1
	mov	x27, x9
	add	w12, w12, w3
	sub	w12, w12, #30
	cmp	w14, w12
	mov	x14, x11
	mov	x12, x8
	b.le	.LBB3_133
// %bb.129:                             //   in Loop: Header=BB3_107 Depth=2
	mov	x14, x24
	mov	w12, #1                         // =0x1
	mov	x27, x16
	b	.LBB3_133
.LBB3_130:                              //   in Loop: Header=BB3_107 Depth=2
	eor	x12, x16, x14
	rbit	x12, x12
	clz	x12, x12
	lsr	x12, x12, #3
	mov	w14, #65531                     // =0xfffb
	movk	w14, #32767, lsl #16
	cmp	w12, w14
	b.ls	.LBB3_128
.LBB3_131:                              //   in Loop: Header=BB3_107 Depth=2
	mov	x14, x11
	mov	x12, x8
.LBB3_132:                              //   in Loop: Header=BB3_107 Depth=2
	mov	x27, x9
.LBB3_133:                              //   in Loop: Header=BB3_107 Depth=2
	mov	x19, x30
	sub	x8, x24, x5
	ldrb	w11, [x19, #5]!
	ldr	w9, [x2, w8, uxtw #2]
	and	w11, w11, #0x7
	lsr	w11, w9, w11
	tbz	w11, #0, .LBB3_182
// %bb.134:                             //   in Loop: Header=BB3_107 Depth=2
	cmp	w27, #3
	mov	w16, #3                         // =0x3
	mov	x11, xzr
	csel	w16, w27, w16, hi
	sub	x7, x30, #2
	mov	w22, #4                         // =0x4
	sub	w3, w16, #3
	add	w8, w3, w8
	ldr	w4, [x2, w8, uxtw #2]
	cmp	w4, w9
	csel	w8, w3, wzr, lo
	add	x3, x24, w8, sxtw
	ldrb	w3, [x3, #4]
	and	w3, w3, #0x7
	lsl	w25, w1, w3
	csel	w3, w4, w9, lo
	b	.LBB3_138
.LBB3_135:                              //   in Loop: Header=BB3_138 Depth=3
	mov	x11, x9
.LBB3_136:                              //   in Loop: Header=BB3_138 Depth=3
	mov	w16, w3
.LBB3_137:                              //   in Loop: Header=BB3_138 Depth=3
	subs	w22, w22, #1
	mov	w3, w20
	b.eq	.LBB3_178
.LBB3_138:                              //   Parent Loop BB3_13 Depth=1
                                        //     Parent Loop BB3_107 Depth=2
                                        // =>    This Loop Header: Depth=3
                                        //         Child Loop BB3_143 Depth 4
	lsr	w9, w3, #15
	subs	w6, w9, w8
	b.ls	.LBB3_178
// %bb.139:                             //   in Loop: Header=BB3_138 Depth=3
	add	x4, x5, w9, uxtw
	ldr	w20, [x2, w9, uxtw #2]
	sub	x9, x4, w8, uxtw
	tst	w20, w25
	b.eq	.LBB3_162
// %bb.140:                             //   in Loop: Header=BB3_138 Depth=3
	mov	w3, w16
	add	x4, x9, x3
	ldr	w3, [x7, x3]
	ldur	w4, [x4, #-3]
	cmp	w4, w3
	b.ne	.LBB3_137
// %bb.141:                             //   in Loop: Header=BB3_138 Depth=3
	ldr	x3, [x9]
	ldr	x4, [x24]
	cmp	x3, x4
	b.ne	.LBB3_146
// %bb.142:                             //   in Loop: Header=BB3_138 Depth=3
	mov	x4, xzr
.LBB3_143:                              //   Parent Loop BB3_13 Depth=1
                                        //     Parent Loop BB3_107 Depth=2
                                        //       Parent Loop BB3_138 Depth=3
                                        // =>      This Inner Loop Header: Depth=4
	add	x3, x30, x4
	add	x28, x9, x4
	add	x3, x3, #9
	cmp	x3, x18
	b.hs	.LBB3_149
// %bb.144:                             //   in Loop: Header=BB3_143 Depth=4
	ldr	x28, [x28, #8]
	ldr	x3, [x3]
	add	x4, x4, #8
	cmp	x28, x3
	b.eq	.LBB3_143
// %bb.145:                             //   in Loop: Header=BB3_138 Depth=3
	eor	x3, x3, x28
	rbit	x3, x3
	clz	x3, x3
	add	x3, x4, x3, lsr #3
	cmp	w16, w3
	b.lo	.LBB3_147
	b	.LBB3_158
.LBB3_146:                              //   in Loop: Header=BB3_138 Depth=3
	eor	x3, x4, x3
	rbit	x3, x3
	clz	x3, x3
	lsr	x3, x3, #3
	cmp	w16, w3
	b.hs	.LBB3_158
.LBB3_147:                              //   in Loop: Header=BB3_138 Depth=3
	add	w11, w6, w3
	sub	w11, w11, #4
	ldr	w11, [x2, w11, uxtw #2]
	cmp	w20, w11
	b.ls	.LBB3_135
// %bb.148:                             //   in Loop: Header=BB3_138 Depth=3
	sub	w8, w3, #4
	mov	w20, w11
	mov	x11, x9
	b	.LBB3_161
.LBB3_149:                              //   in Loop: Header=BB3_138 Depth=3
	ldur	x0, [x29, #-8]                  // 8-byte Folded Reload
	add	x28, x28, #8
	cmp	x3, x0
	b.hs	.LBB3_152
// %bb.150:                             //   in Loop: Header=BB3_138 Depth=3
	ldr	w0, [x28]
	ldr	w1, [x3]
	cmp	w0, w1
	b.ne	.LBB3_152
// %bb.151:                             //   in Loop: Header=BB3_138 Depth=3
	add	x0, x30, x4
	add	x1, x9, x4
	add	x3, x0, #13
	add	x28, x1, #12
.LBB3_152:                              //   in Loop: Header=BB3_138 Depth=3
	ldur	x0, [x29, #-16]                 // 8-byte Folded Reload
	cmp	x3, x0
	b.hs	.LBB3_155
// %bb.153:                             //   in Loop: Header=BB3_138 Depth=3
	ldrh	w0, [x28]
	ldrh	w1, [x3]
	cmp	w0, w1
	b.ne	.LBB3_155
// %bb.154:                             //   in Loop: Header=BB3_138 Depth=3
	add	x3, x3, #2
	add	x28, x28, #2
.LBB3_155:                              //   in Loop: Header=BB3_138 Depth=3
	cmp	x3, x10
	b.hs	.LBB3_157
// %bb.156:                             //   in Loop: Header=BB3_138 Depth=3
	mov	x0, x3
	ldrb	w1, [x28]
	ldrb	w4, [x0], #1
	cmp	w1, w4
	csel	x3, x3, x0, ne
.LBB3_157:                              //   in Loop: Header=BB3_138 Depth=3
	mov	w1, #1                          // =0x1
	sub	x3, x3, x24
	cmp	w16, w3
	b.lo	.LBB3_147
.LBB3_158:                              //   in Loop: Header=BB3_138 Depth=3
	sub	w9, w16, #3
	cmp	w8, w9
	b.eq	.LBB3_137
// %bb.159:                             //   in Loop: Header=BB3_138 Depth=3
	add	w0, w6, w16
	sub	w0, w0, #3
	ldr	w3, [x2, w0, uxtw #2]
	cmp	w20, w3
	b.ls	.LBB3_137
// %bb.160:                             //   in Loop: Header=BB3_138 Depth=3
	mov	w8, w9
	mov	w20, w3
	mov	w3, w16
.LBB3_161:                              //   in Loop: Header=BB3_138 Depth=3
	ldrb	w9, [x19, w8, sxtw]
	and	w9, w9, #0x7
	lsl	w25, w1, w9
	b	.LBB3_136
.LBB3_162:                              //   in Loop: Header=BB3_107 Depth=2
	ldr	x4, [x9]
	ldr	x6, [x24]
	cmp	x4, x6
	b.ne	.LBB3_167
// %bb.163:                             //   in Loop: Header=BB3_107 Depth=2
	sub	x7, x5, w8, uxtw
	mov	w19, w3
	mov	x6, xzr
	add	x3, x7, x19, lsr #15
.LBB3_164:                              //   Parent Loop BB3_13 Depth=1
                                        //     Parent Loop BB3_107 Depth=2
                                        // =>    This Inner Loop Header: Depth=3
	add	x8, x30, x6
	add	x8, x8, #9
	cmp	x8, x18
	b.hs	.LBB3_168
// %bb.165:                             //   in Loop: Header=BB3_164 Depth=3
	add	x4, x3, x6
	ldr	x8, [x8]
	add	x6, x6, #8
	ldr	x4, [x4, #8]
	cmp	x4, x8
	b.eq	.LBB3_164
// %bb.166:                             //   in Loop: Header=BB3_107 Depth=2
	eor	x8, x8, x4
	rbit	x8, x8
	clz	x8, x8
	add	x8, x6, x8, lsr #3
	b	.LBB3_177
.LBB3_167:                              //   in Loop: Header=BB3_107 Depth=2
	eor	x8, x6, x4
	rbit	x8, x8
	clz	x8, x8
	lsr	x8, x8, #3
	b	.LBB3_177
.LBB3_168:                              //   in Loop: Header=BB3_107 Depth=2
	add	x3, x7, x19, lsr #15
	ldur	x0, [x29, #-8]                  // 8-byte Folded Reload
	add	x3, x3, x6
	cmp	x8, x0
	add	x3, x3, #8
	b.hs	.LBB3_171
// %bb.169:                             //   in Loop: Header=BB3_107 Depth=2
	ldr	w4, [x3]
	ldr	w20, [x8]
	cmp	w4, w20
	b.ne	.LBB3_171
// %bb.170:                             //   in Loop: Header=BB3_107 Depth=2
	add	x8, x7, x19, lsr #15
	add	x3, x30, x6
	add	x4, x8, x6
	add	x8, x3, #13
	add	x3, x4, #12
.LBB3_171:                              //   in Loop: Header=BB3_107 Depth=2
	ldur	x0, [x29, #-16]                 // 8-byte Folded Reload
	cmp	x8, x0
	b.hs	.LBB3_174
// %bb.172:                             //   in Loop: Header=BB3_107 Depth=2
	ldrh	w4, [x3]
	ldrh	w6, [x8]
	cmp	w4, w6
	b.ne	.LBB3_174
// %bb.173:                             //   in Loop: Header=BB3_107 Depth=2
	add	x8, x8, #2
	add	x3, x3, #2
.LBB3_174:                              //   in Loop: Header=BB3_107 Depth=2
	cmp	x8, x10
	b.hs	.LBB3_176
// %bb.175:                             //   in Loop: Header=BB3_107 Depth=2
	mov	x4, x8
	ldrb	w3, [x3]
	ldrb	w6, [x4], #1
	cmp	w3, w6
	csel	x8, x8, x4, ne
.LBB3_176:                              //   in Loop: Header=BB3_107 Depth=2
	sub	x8, x8, x24
.LBB3_177:                              //   in Loop: Header=BB3_107 Depth=2
	cmp	w16, w8
	csel	x11, x9, x11, lo
	csel	w16, w16, w8, hi
.LBB3_178:                              //   in Loop: Header=BB3_107 Depth=2
	cbz	x11, .LBB3_186
// %bb.179:                             //   in Loop: Header=BB3_107 Depth=2
	cmp	w16, #4
	b.lt	.LBB3_186
// %bb.180:                             //   in Loop: Header=BB3_107 Depth=2
	sub	x8, x24, x11
	add	x8, x8, #3
.LBB3_181:                              //   in Loop: Header=BB3_107 Depth=2
	add	w9, w12, #1
	add	w11, w8, #1
	add	w0, w27, w27, lsl #2
	add	w1, w16, w16, lsl #2
	clz	w9, w9
	clz	w11, w11
	add	w9, w9, w0
	add	x13, x13, #1
	mov	x30, x24
	add	w11, w1, w11
	sub	w0, w9, #30
	mov	w9, w16
	sub	w11, w11, #31
	mov	w1, #1                          // =0x1
	cmp	w11, w0
	mov	x11, x24
	b.gt	.LBB3_107
	b	.LBB3_186
.LBB3_182:                              //   in Loop: Header=BB3_107 Depth=2
	cmp	w27, #3
	b.hi	.LBB3_186
// %bb.183:                             //   in Loop: Header=BB3_107 Depth=2
	lsr	x9, x9, #15
	ldr	w11, [x24]
	ldr	w16, [x5, x9]
	cmp	w16, w11
	b.ne	.LBB3_186
// %bb.184:                             //   in Loop: Header=BB3_107 Depth=2
	sub	w8, w8, w9
	mov	w16, #4                         // =0x4
	add	w8, w8, #3
	b	.LBB3_181
.LBB3_185:                              //   in Loop: Header=BB3_13 Depth=1
	mov	x14, x11
	mov	x12, x8
	mov	x27, x9
.LBB3_186:                              //   in Loop: Header=BB3_13 Depth=1
	cmp	x12, #4
	b.lo	.LBB3_193
// %bb.187:                             //   in Loop: Header=BB3_13 Depth=1
	ldur	x0, [x29, #-32]                 // 8-byte Folded Reload
	cmp	x14, x23
	b.ls	.LBB3_194
// %bb.188:                             //   in Loop: Header=BB3_13 Depth=1
	sub	x8, x14, x12
	ldr	x16, [sp, #56]                  // 8-byte Folded Reload
	add	x8, x8, #3
	cmp	x8, x16
	b.ls	.LBB3_194
// %bb.189:                             //   in Loop: Header=BB3_13 Depth=1
	mov	w8, #2                          // =0x2
	sub	x9, x8, x12
	mov	x8, x14
.LBB3_190:                              //   Parent Loop BB3_13 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	ldrb	w11, [x8, #-1]!
	ldrb	w13, [x14, x9]
	cmp	w11, w13
	b.ne	.LBB3_194
// %bb.191:                             //   in Loop: Header=BB3_190 Depth=2
	cmp	x8, x23
	add	x27, x27, #1
	b.ls	.LBB3_195
// %bb.192:                             //   in Loop: Header=BB3_190 Depth=2
	add	x11, x14, x9
	mov	x14, x8
	cmp	x11, x16
	b.hi	.LBB3_190
	b	.LBB3_195
.LBB3_193:                              //   in Loop: Header=BB3_13 Depth=1
	ldur	x0, [x29, #-32]                 // 8-byte Folded Reload
	ldr	x9, [x21, #24]
	cmp	x14, x0
	sub	x8, x14, x23
	b.hi	.LBB3_33
	b	.LBB3_196
.LBB3_194:                              //   in Loop: Header=BB3_13 Depth=1
	mov	x8, x14
.LBB3_195:                              //   in Loop: Header=BB3_13 Depth=1
	sub	w9, w12, #3
	mov	w26, w15
	mov	x14, x8
	mov	w15, w9
	ldr	x9, [x21, #24]
	cmp	x8, x0
	sub	x8, x8, x23
	b.hi	.LBB3_33
.LBB3_196:                              //   in Loop: Header=BB3_13 Depth=1
	ldr	q0, [x23]
	cmp	x8, #16
	str	q0, [x9]
	ldr	x11, [x21, #24]
	b.ls	.LBB3_231
// %bb.197:                             //   in Loop: Header=BB3_13 Depth=1
	ldr	q0, [x23, #16]
	cmp	x8, #33
	str	q0, [x11, #16]
	b.lt	.LBB3_200
// %bb.198:                             //   in Loop: Header=BB3_13 Depth=1
	add	x9, x11, x8
	add	x11, x11, #32
	add	x13, x23, #48
.LBB3_199:                              //   Parent Loop BB3_13 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	ldur	q0, [x13, #-16]
	str	q0, [x11]
	ldr	q0, [x13], #32
	str	q0, [x11, #16]
	add	x11, x11, #32
	cmp	x11, x9
	b.lo	.LBB3_199
.LBB3_200:                              //   in Loop: Header=BB3_13 Depth=1
	ldr	x9, [x21, #24]
	cmp	x8, #16, lsl #12                // =65536
	add	x11, x9, x8
	ldr	x9, [x21, #8]
	str	x11, [x21, #24]
	b.hs	.LBB3_233
// %bb.201:                             //   in Loop: Header=BB3_13 Depth=1
	sub	x11, x27, #3
	strh	w8, [x9, #4]
	cmp	x11, #16, lsl #12               // =65536
	str	w12, [x9]
	b.hs	.LBB3_232
.LBB3_202:                              //   in Loop: Header=BB3_13 Depth=1
	add	x8, x9, #8
	add	x23, x14, x27
	strh	w11, [x9, #6]
	str	x8, [x21, #8]
	cbz	w26, .LBB3_230
.LBB3_203:                              //   in Loop: Header=BB3_13 Depth=1
	cmp	x23, x17
	b.hi	.LBB3_230
// %bb.204:                             //   in Loop: Header=BB3_13 Depth=1
	mov	w11, w15
	mov	w9, w26
.LBB3_205:                              //   Parent Loop BB3_13 Depth=1
                                        // =>  This Loop Header: Depth=2
                                        //       Child Loop BB3_209 Depth 3
	mov	w12, w9
	mov	w15, w9
	ldr	w13, [x23]
	sub	x9, x23, x12
	ldr	w9, [x9]
	cmp	w13, w9
	mov	w9, w11
	b.ne	.LBB3_229
// %bb.206:                             //   in Loop: Header=BB3_205 Depth=2
	add	x11, x23, #4
	neg	x16, x12
	cmp	x11, x18
	add	x13, x11, x16
	b.hs	.LBB3_212
// %bb.207:                             //   in Loop: Header=BB3_205 Depth=2
	ldr	x12, [x13]
	ldr	x13, [x11]
	cmp	x12, x13
	b.ne	.LBB3_226
// %bb.208:                             //   in Loop: Header=BB3_205 Depth=2
	add	x12, x23, #12
	mov	x14, xzr
	add	x13, x12, x16
.LBB3_209:                              //   Parent Loop BB3_13 Depth=1
                                        //     Parent Loop BB3_205 Depth=2
                                        // =>    This Inner Loop Header: Depth=3
	cmp	x12, x18
	b.hs	.LBB3_213
// %bb.210:                             //   in Loop: Header=BB3_209 Depth=3
	ldr	x16, [x13], #8
	add	x14, x14, #8
	ldr	x3, [x12], #8
	cmp	x16, x3
	b.eq	.LBB3_209
// %bb.211:                             //   in Loop: Header=BB3_205 Depth=2
	eor	x11, x3, x16
	rbit	x11, x11
	clz	x11, x11
	add	x11, x14, x11, lsr #3
	cmp	x23, x0
	b.ls	.LBB3_222
	b	.LBB3_223
.LBB3_212:                              //   in Loop: Header=BB3_205 Depth=2
	mov	x12, x11
.LBB3_213:                              //   in Loop: Header=BB3_205 Depth=2
	ldur	x14, [x29, #-8]                 // 8-byte Folded Reload
	cmp	x12, x14
	b.hs	.LBB3_216
	ldr	w14, [x13]
	ldr	w16, [x12]
	cmp	w14, w16
	b.ne	.LBB3_216
	add	x12, x12, #4
	add	x13, x13, #4
.LBB3_216:                              //   in Loop: Header=BB3_205 Depth=2
	ldur	x14, [x29, #-16]                // 8-byte Folded Reload
	cmp	x12, x14
	b.hs	.LBB3_219
// %bb.217:                             //   in Loop: Header=BB3_205 Depth=2
	ldrh	w14, [x13]
	ldrh	w16, [x12]
	cmp	w14, w16
	b.ne	.LBB3_219
// %bb.218:                             //   in Loop: Header=BB3_205 Depth=2
	add	x12, x12, #2
	add	x13, x13, #2
.LBB3_219:                              //   in Loop: Header=BB3_205 Depth=2
	cmp	x12, x10
	b.hs	.LBB3_221
// %bb.220:                             //   in Loop: Header=BB3_205 Depth=2
	mov	x14, x12
	ldrb	w13, [x13]
	ldrb	w16, [x14], #1
	cmp	w13, w16
	csel	x12, x12, x14, ne
.LBB3_221:                              //   in Loop: Header=BB3_205 Depth=2
	sub	x11, x12, x11
	cmp	x23, x0
	b.hi	.LBB3_223
.LBB3_222:                              //   in Loop: Header=BB3_205 Depth=2
	ldr	x8, [x21, #24]
	ldr	q0, [x23]
	str	q0, [x8]
	ldr	x8, [x21, #8]
.LBB3_223:                              //   in Loop: Header=BB3_205 Depth=2
	add	x12, x11, #1
	strh	wzr, [x8, #4]
	cmp	x12, #16, lsl #12               // =65536
	str	w1, [x8]
	b.hs	.LBB3_227
// %bb.224:                             //   in Loop: Header=BB3_205 Depth=2
	add	x11, x23, x11
	strh	w12, [x8, #6]
	add	x8, x8, #8
	add	x23, x11, #4
	str	x8, [x21, #8]
	cbz	w9, .LBB3_228
.LBB3_225:                              //   in Loop: Header=BB3_205 Depth=2
	cmp	x23, x17
	mov	w11, w15
	mov	w26, w9
	mov	x30, x23
	b.ls	.LBB3_205
	b	.LBB3_12
.LBB3_226:                              //   in Loop: Header=BB3_205 Depth=2
	eor	x11, x13, x12
	rbit	x11, x11
	clz	x11, x11
	lsr	x11, x11, #3
	cmp	x23, x0
	b.ls	.LBB3_222
	b	.LBB3_223
.LBB3_227:                              //   in Loop: Header=BB3_205 Depth=2
	ldr	x13, [x21]
	mov	w14, #2                         // =0x2
	sub	x13, x8, x13
	lsr	x13, x13, #3
	stp	w14, w13, [x21, #72]
	add	x11, x23, x11
	strh	w12, [x8, #6]
	add	x8, x8, #8
	add	x23, x11, #4
	str	x8, [x21, #8]
	cbnz	w9, .LBB3_225
.LBB3_228:                              //   in Loop: Header=BB3_13 Depth=1
	mov	w26, w9
	mov	x30, x23
	b	.LBB3_12
.LBB3_229:                              //   in Loop: Header=BB3_13 Depth=1
	mov	w26, w15
	mov	w15, w9
.LBB3_230:                              //   in Loop: Header=BB3_13 Depth=1
	mov	x30, x23
	b	.LBB3_12
.LBB3_231:                              //   in Loop: Header=BB3_13 Depth=1
	add	x11, x11, x8
	ldr	x9, [x21, #8]
	str	x11, [x21, #24]
	sub	x11, x27, #3
	strh	w8, [x9, #4]
	cmp	x11, #16, lsl #12               // =65536
	str	w12, [x9]
	b.lo	.LBB3_202
.LBB3_232:                              //   in Loop: Header=BB3_13 Depth=1
	ldr	x8, [x21]
	mov	w12, #2                         // =0x2
	sub	x8, x9, x8
	lsr	x8, x8, #3
	stp	w12, w8, [x21, #72]
	add	x8, x9, #8
	add	x23, x14, x27
	strh	w11, [x9, #6]
	str	x8, [x21, #8]
	cbnz	w26, .LBB3_203
	b	.LBB3_230
.LBB3_233:                              //   in Loop: Header=BB3_13 Depth=1
	ldr	x11, [x21]
	sub	x11, x9, x11
	lsr	x11, x11, #3
	stp	w1, w11, [x21, #72]
	sub	x11, x27, #3
	strh	w8, [x9, #4]
	cmp	x11, #16, lsl #12               // =65536
	str	w12, [x9]
	b.lo	.LBB3_202
	b	.LBB3_232
.LBB3_234:
	mov	w26, wzr
	mov	w15, wzr
	mov	x23, x8
.LBB3_235:
	ldr	x9, [sp, #24]                   // 8-byte Folded Reload
	ldr	x11, [sp, #8]                   // 8-byte Folded Reload
	mov	w8, #1                          // =0x1
	ldr	w12, [sp, #36]
	sub	x0, x10, x23
	sub	w9, w11, w9
	ldr	w11, [sp, #20]
	lsl	w8, w8, w11
	sub	w11, w9, w12
	cmp	w11, w8
	ldr	w11, [sp, #32]
	sub	w8, w9, w8
	ccmp	w11, #0, #0, hi
	csel	w8, w8, w12, eq
	sub	w8, w9, w8
	ldp	w9, w12, [sp, #40]
	cmp	w9, w8
	csel	w9, w9, wzr, hi
	cmp	w12, w8
	csel	w11, w12, wzr, hi
	cmp	w15, #0
	csel	w11, w15, w11, ne
	ccmp	w12, w8, #0, ne
	csel	w8, w12, w9, hi
	cmp	w26, #0
	ldr	x9, [sp, #48]
	csel	w8, w8, w26, eq
	stp	w11, w8, [x9]
	.cfi_def_cfa wsp, 208
	ldp	x20, x19, [sp, #192]
	ldp	x22, x21, [sp, #176]
	ldp	x24, x23, [sp, #160]
	ldp	x26, x25, [sp, #144]
	ldp	x28, x27, [sp, #128]
	ldp	x29, x30, [sp, #112]
	add	sp, sp, #208
	.cfi_def_cfa_offset 0
	.cfi_restore w19
	.cfi_restore w20
	.cfi_restore w21
	.cfi_restore w22
	.cfi_restore w23
	.cfi_restore w24
	.cfi_restore w25
	.cfi_restore w26
	.cfi_restore w27
	.cfi_restore w28
	.cfi_restore w30
	.cfi_restore w29
	ret
.LBB3_236:
	.cfi_restore_state
	mov	w0, wzr
	bl	exit
	.cfi_endproc
