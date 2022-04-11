.section .text

_userelf: .incbin "../../user/test.elf"
_userelf_end:

userelf: .dword _userelf
userelfsize: .dword _userelf_end - _userelf
.global userelfsize
.global userelf
