#if defined(__aarch64__)
.text
.global predict_min_match
.p2align	2
.type predict_min_match, %function

predict_min_match:                      
	.cfi_startproc

	cmp	x1, #1024
	b.hs	.handle_large_input

	mov	w0, #6                          
	ret

// 处理大输入（需要分配栈空间和初始化）
.handle_large_input:
	stp	x29, x30, [sp, #-64]!           
	.cfi_def_cfa_offset 64
	stp	x24, x23, [sp, #16]             
	stp	x22, x21, [sp, #32]             
	stp	x20, x19, [sp, #48]             
	mov	x29, sp
	.cfi_def_cfa w29, 64
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w21, -24
	.cfi_offset w22, -32
	.cfi_offset w23, -40
	.cfi_offset w24, -48
	.cfi_offset w30, -56
	.cfi_offset w29, -64
	.cfi_remember_state
	mov	w8, #57600                      
	mov	x19, x2
	mov	x20, x0
	cmp	x1, x8
	mov	x0, x2
	mov	x24, x1
	csel	x21, x1, x8, lo
	mov	w1, wzr
	mov	w2, #65536                      
	bl	memset
	add	x9, x20, x24
	mov	w15, #31153                     
	mov	x22, xzr
	mov	x23, xzr
	mov	x8, xzr
	sub	x10, x21, #8
	sub	x11, x9, #7
	sub	x12, x9, #3
	sub	x13, x9, #1
	add	x14, x20, #12
	movk	w15, #40503, lsl #16
	b	.main_search_loop

// 计算匹配长度并更新统计
.calc_match_length:
	sub	x16, x17, x16

.update_match_stats:
	add	w16, w16, #4
	add	x23, x23, #1
	sub	x17, x16, #1
	cmp	w16, #4
	add	x22, x16, x22
	csel	x17, x17, xzr, hi
	add	x8, x17, x8

.continue_search:
	add	x8, x8, #4
	cmp	x8, x10
	b.hs	.cleanup_and_return

// 主搜索循环
.main_search_loop:
	add	x16, x20, x8
	ldr	w18, [x16]
	mul	w17, w18, w15
	lsr	w17, w17, #18
	ubfiz	x0, x17, #2, #32
	ldr	w17, [x19, x0]
	str	w8, [x19, x0]
	cbz	w17, .continue_search

// 检查匹配
.check_match:
	add	x0, x20, x17
	ldr	w1, [x0]
	cmp	w1, w18
	b.ne	.continue_search

// 找到潜在匹配，开始详细比较
.start_comparison:
	add	x16, x16, #4
	add	x18, x0, #4
	cmp	x11, x16
	b.ls	.compare_small_block

// 比较8字节块
.compare_8bytes:
	ldr	x18, [x18]
	ldr	x0, [x16]
	eor	x18, x0, x18
	cbz	x18, .compare_long_block

// 计算不匹配位置
.calc_mismatch_pos:
	rbit	x16, x18
	clz	x16, x16
	lsr	x16, x16, #3
	b	.update_match_stats

// 比较小数据块
.compare_small_block:
	mov	x17, x16
	cmp	x16, x12
	b.hs	.compare_remaining

.compare_word:
	ldr	w0, [x18]
	ldr	w1, [x17]
	cmp	w0, w1
	add	x0, x17, #4
	add	x1, x18, #4
	csel	x18, x18, x1, ne
	csel	x17, x17, x0, ne

.compare_remaining:
	cmp	x17, x13
	b.lo	.compare_halfword

// 比较到末尾
.compare_to_end:
	cmp	x17, x9
	b.hs	.calc_match_length
	b	.compare_last_byte

// 半字比较
.compare_halfword:
	ldrh	w0, [x18]
	ldrh	w1, [x17]
	cmp	w0, w1
	add	x0, x17, #2
	add	x1, x18, #2
	csel	x18, x18, x1, ne
	csel	x17, x17, x0, ne
	cmp	x17, x9
	b.hs	.calc_match_length

// 逐字节比较
.compare_last_byte:
	mov	x0, x17
	ldrb	w18, [x18]
	ldrb	w1, [x0], #1
	cmp	w18, w1
	csel	x17, x17, x0, ne
	b	.calc_match_length

// 比较长匹配块
.compare_long_block:
	mov	x0, xzr
	add	x18, x14, x17
	add	x1, x14, x8

.long_comparison_loop:
	add	x2, x1, x0
	cmp	x2, x11
	b.hs	.long_comparison_done

// 比较8字节块
.compare_block_8bytes:
	ldr	x3, [x18, x0]
	ldr	x2, [x2]
	add	x0, x0, #8
	eor	x2, x2, x3
	cbz	x2, .long_comparison_loop

// 找到不匹配
.mismatch_found:
	rbit	x16, x2
	clz	x16, x16
	add	x16, x0, x16, lsr #3
	b	.update_match_stats

// 长比较完成
.long_comparison_done:
	add	x17, x14, x17
	add	x1, x14, x8
	add	x18, x17, x0
	add	x17, x1, x0
	cmp	x17, x12
	b.lo	.compare_word
	b	.compare_remaining

// 清理并返回结果
.cleanup_and_return:
	mov	x0, x19
	mov	w1, wzr
	mov	w2, #65536                     
	bl	memset
	cbz	x23, .return_default

// 计算返回值
.calculate_result:
	add	x8, x23, x23, lsl #1
	sub	x9, x21, x22
	ucvtf	s0, x21
	add	x8, x9, x8
	cmp	x8, #1
	csinc	x8, x8, xzr, hi
	ucvtf	s1, x8
	fdiv	s0, s0, s1
	fmov	s1, #6.00000000
	fcmp	s0, s1
	b.le	.check_heuristic

// 返回结果8
.return_8:
	mov	w0, #8                          

.return_cleanup:
	.cfi_def_cfa wsp, 64
	ldp	x20, x19, [sp, #48]             
	ldp	x22, x21, [sp, #32]             
	ldp	x24, x23, [sp, #16]            
	ldp	x29, x30, [sp], #64          
	.cfi_def_cfa_offset 0
	.cfi_restore w19
	.cfi_restore w20
	.cfi_restore w21
	.cfi_restore w22
	.cfi_restore w23
	.cfi_restore w24
	.cfi_restore w30
	.cfi_restore w29
	ret

// 默认返回5
.return_default:
	.cfi_restore_state
	.cfi_remember_state
	mov	w0, #5                         
	.cfi_def_cfa wsp, 64
	ldp	x20, x19, [sp, #48]             
	ldp	x22, x21, [sp, #32]            
	ldp	x24, x23, [sp, #16]           
	ldp	x29, x30, [sp], #64           
	.cfi_def_cfa_offset 0
	.cfi_restore w19
	.cfi_restore w20
	.cfi_restore w21
	.cfi_restore w22
	.cfi_restore w23
	.cfi_restore w24
	.cfi_restore w30
	.cfi_restore w29
	ret

// 启发式检查
.check_heuristic:
	.cfi_restore_state
	mov	w8, #100                        
	mov	w0, #7                         
	mul	x8, x22, x8
	udiv	x8, x8, x21
	cmp	w8, #60
	b.hi	.return_cleanup

// 最终检查
.final_check:
	udiv	x9, x22, x23
	cmp	w9, #13
	b.hi	.return_cleanup

// 决策返回值
.decision_return:
	cmp	w9, #8
	mov	w9, #40                      
	ccmp	w8, w9, #2, ls
	mov	w8, #5                         
	cinc	w0, w8, hi
	.cfi_def_cfa wsp, 64
	ldp	x20, x19, [sp, #48]           
	ldp	x22, x21, [sp, #32]        
	ldp	x24, x23, [sp, #16]         
	ldp	x29, x30, [sp], #64         
	.cfi_def_cfa_offset 0
	.cfi_restore w19
	.cfi_restore w20
	.cfi_restore w21
	.cfi_restore w22
	.cfi_restore w23
	.cfi_restore w24
	.cfi_restore w30
	.cfi_restore w29
	ret

.Lfunc_end59:
	.size	predict_min_match, .Lfunc_end59-predict_min_match
	.cfi_endproc
#endif