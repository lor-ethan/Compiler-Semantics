			.text
			.globl			__start
__start:
			jal			_main
			li			$v0, 			10
			syscall
_main:
			li			$v0, 			4
			la			$a0, 			L1
			syscall
			li			$v0, 			5
			syscall
			move			$t0, 			$v0
			sw			$t0, 			_p
L10:
			lw			$t0, 			_p
			li			$t1, 			1
			ble			$t0, 			$t1, 			L2
			li			$v0, 			4
			la			$a0, 			L3
			syscall
			lw			$t0, 			_p
			li			$v0, 			1
			move			$a0, 			$t0
			syscall
			li			$v0, 			4
			la			$a0, 			L4
			syscall
			li			$t0, 			2
			sw			$t0, 			_d
L8:
			lw			$t0, 			_d
			lw			$t1, 			_p
			li			$t2, 			2
			div			$t3, 			$t1, 			$t2
			bge			$t0, 			$t3, 			L5
			lw			$t0, 			_p
			lw			$t1, 			_d
			div			$t2, 			$t0, 			$t1
			sw			$t2, 			_q
			lw			$t0, 			_q
			lw			$t1, 			_d
			mul			$t2, 			$t0, 			$t1
			lw			$t0, 			_p
			bne			$t2, 			$t0, 			L6
			lw			$t0, 			_d
			li			$v0, 			1
			move			$a0, 			$t0
			syscall
			li			$v0, 			4
			la			$a0, 			L7
			syscall
L6:
			lw			$t0, 			_d
			li			$t1, 			1
			add			$t2, 			$t0, 			$t1
			sw			$t2, 			_d
			b			L8
L5:
			li			$v0, 			4
			la			$a0, 			L9
			syscall
			li			$v0, 			4
			la			$a0, 			L1
			syscall
			li			$v0, 			5
			syscall
			move			$t0, 			$v0
			sw			$t0, 			_p
			b			L10
L2:
			jr			$ra
			.data
_d:			.word			0
_p:			.word			0
_q:			.word			0
L7:			.asciiz			" "
L4:			.asciiz			" are "
L1:			.asciiz			"> "
L9:			.asciiz			"\n"
L3:			.asciiz			"factors of "
