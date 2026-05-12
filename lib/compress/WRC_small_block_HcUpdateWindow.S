#if defined(__aarch64__)
.text
.global WRC_small_block_HcUpdateWindow
.p2align	2                               // -- Begin function WRC_small_block_HcUpdateWindow
	.type	WRC_small_block_HcUpdateWindow,@function
WRC_small_block_HcUpdateWindow:         // @WRC_small_block_HcUpdateWindow
	.cfi_startproc
// %bb.0:
	mov	w8, #56507                      // =0xdcbb
	cmp	w3, #8
	mov	w9, w3
	movk	w8, #7100, lsl #16
	b.lo	.LBB7_4
// %bb.1:
	mov	x12, xzr
	add	x10, x0, #1536, lsl #12         // =6291456
	add	x11, x0, #16
.LBB7_2:                                // =>This Inner Loop Header: Depth=1
	add	x13, x2, x12
	add	w17, w12, #1
	ldr	w14, [x13]
	mul	w14, w14, w8
	lsr	w14, w14, #17
	ubfiz	x14, x14, #2, #32
	add	x15, x10, x14
	prfm	pstl1keep, [x15]
	ldur	w15, [x13, #1]
	ldr	w16, [x10, x14]
	mul	w15, w15, w8
	stur	w16, [x11, #-16]
	str	w12, [x10, x14]
	lsr	w15, w15, #17
	ubfiz	x15, x15, #2, #32
	add	x16, x10, x15
	prfm	pstl1keep, [x16]
	ldur	w14, [x13, #2]
	ldr	w16, [x10, x15]
	mul	w14, w14, w8
	stur	w16, [x11, #-12]
	str	w17, [x10, x15]
	add	w17, w12, #2
	lsr	w14, w14, #17
	ubfiz	x14, x14, #2, #32
	add	x16, x10, x14
	prfm	pstl1keep, [x16]
	ldur	w15, [x13, #3]
	ldr	w16, [x10, x14]
	mul	w15, w15, w8
	stur	w16, [x11, #-8]
	str	w17, [x10, x14]
	add	w17, w12, #3
	lsr	w15, w15, #17
	ubfiz	x15, x15, #2, #32
	add	x16, x10, x15
	prfm	pstl1keep, [x16]
	ldr	w14, [x13, #4]
	ldr	w16, [x10, x15]
	mul	w14, w14, w8
	stur	w16, [x11, #-4]
	str	w17, [x10, x15]
	add	w17, w12, #4
	lsr	w14, w14, #17
	ubfiz	x14, x14, #2, #32
	add	x16, x10, x14
	prfm	pstl1keep, [x16]
	ldur	w15, [x13, #5]
	ldr	w16, [x10, x14]
	mul	w15, w15, w8
	str	w16, [x11]
	str	w17, [x10, x14]
	add	w17, w12, #5
	lsr	w15, w15, #17
	ubfiz	x15, x15, #2, #32
	add	x16, x10, x15
	prfm	pstl1keep, [x16]
	ldur	w14, [x13, #6]
	ldr	w16, [x10, x15]
	mul	w14, w14, w8
	str	w16, [x11, #4]
	str	w17, [x10, x15]
	lsr	w14, w14, #17
	ubfiz	x14, x14, #2, #32
	add	x16, x10, x14
	prfm	pstl1keep, [x16]
	ldur	w13, [x13, #7]
	ldr	w15, [x10, x14]
	mul	w13, w13, w8
	str	w15, [x11, #8]
	lsr	w13, w13, #17
	ubfiz	x16, x13, #2, #32
	add	w13, w12, #6
	str	w13, [x10, x14]
	add	x13, x12, #8
	add	x15, x10, x16
	prfm	pstl1keep, [x15]
	ldr	w14, [x10, x16]
	add	x15, x12, #15
	add	w12, w12, #7
	cmp	x15, x9
	str	w14, [x11, #12]
	add	x11, x11, #32
	str	w12, [x10, x16]
	mov	x12, x13
	b.lo	.LBB7_2
// %bb.3:
	add	x2, x2, x13
	add	w10, w13, #4
	cmp	w10, w3
	b.lo	.LBB7_5
	b	.LBB7_7
.LBB7_4:
	mov	w13, wzr
	add	w10, w13, #4
	cmp	w10, w3
	b.hs	.LBB7_7
.LBB7_5:
	add	x10, x0, #1536, lsl #12         // =6291456
	mov	w11, w13
	mov	w12, #6291456                   // =0x600000
.LBB7_6:                                // =>This Inner Loop Header: Depth=1
	ldr	w13, [x2], #1
	add	x15, x11, #5
	cmp	x15, x9
	mul	w13, w13, w8
	lsr	w13, w13, #17
	ubfiz	x13, x13, #2, #32
	add	x14, x10, x13
	add	x13, x0, x13
	prfm	pstl1keep, [x14]
	ldr	w14, [x13, x12]
	str	w14, [x0, x11, lsl #2]
	add	x14, x11, #1
	str	w11, [x13, x12]
	mov	x11, x14
	b.lo	.LBB7_6
.LBB7_7:
	ret
.Lfunc_end7:
	.size	WRC_small_block_HcUpdateWindow, .Lfunc_end7-WRC_small_block_HcUpdateWindow
	.cfi_endproc

.text
.global MEM_read64
	.p2align	2                              
	.type	MEM_read64,%function
MEM_read64:                            
	.cfi_startproc

	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	str	x0, [sp, #8]
	ldr	x8, [sp, #8]
	ldr	x0, [x8]
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
.Lfunc_end193:
	.size	MEM_read64, .Lfunc_end193-MEM_read64
	.cfi_endproc
#endif