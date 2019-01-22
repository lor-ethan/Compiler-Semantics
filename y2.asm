			.text
			.globl			__start
__start:
			jal			_main
			li			$v0, 			10
			syscall
_main:
			li			$t0, 			62
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			li			$v0, 			5
			syscall
			move			$t0, 			$v0
			sw			$t0, 			_x
			li			$t0, 			62
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			li			$v0, 			5
			syscall
			move			$t0, 			$v0
			sw			$t0, 			_y
			lw			$t0, 			_x
			lw			$t1, 			_x
			mul			$t2, 			$t0, 			$t1
			lw			$t0, 			_y
			add			$t1, 			$t2, 			$t0
			sw			$t1, 			_r
			li			$t0, 			122
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			li			$t0, 			61
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			lw			$t0, 			_x
			li			$v0, 			1
			move			$a0, 			$t0
			syscall
			li			$t0, 			10
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			li			$t0, 			121
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			li			$t0, 			61
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			lw			$t0, 			_y
			li			$v0, 			1
			move			$a0, 			$t0
			syscall
			li			$t0, 			10
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			li			$t0, 			114
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			li			$t0, 			61
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			lw			$t0, 			_r
			li			$v0, 			1
			move			$a0, 			$t0
			syscall
			li			$t0, 			10
			li			$v0, 			11
			move			$a0, 			$t0
			syscall
			jr			$ra
			.data
_r:			.word			0
_x:			.word			0
_y:			.word			0
