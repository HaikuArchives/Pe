aaa aad aam adc add addl and andl arpl
bound bsf bsr bswap bt btc btr bts
call cbw cdq clc cld cli clts cpuid cmc cmp cmps cmpsb cmpsd cmpsw cmpxchg cwd cwde
daa das dec div
emms enter
f2xm1 fabs fadd faddp fbld fbstp fchs fclex fcmovb fcmovbe fcmove fcmovnb
fcmovnbe fcmovne fcmovnu fcmovu fcom fcomi fcomip fcomp fcompp fcos fdecstp fdiv
fdivp fdivr fdivrp femms ffree fiadd ficom ficomp fidiv fidivr fild fimul
fincstp finit fist fistp fisub fisubr fld fld1 fldcw fldenv fldl2e fldl2t fldlg2
fldln2 fldpi fldz fmul fmulp fnclex fninit fnop fnsave fnstcw fnstenv fnstsw
fpatan fprem1 fptan frndint frstor fsave fscale fsin fsincos fsqrt fst fstcw
fstenv fstp fstsw fsub fsubp fsubr fsubrp ftst fucom fucomi fucomip fucomp
fucompp fwait fxch fxtract fyl2xp1
hlt
idiv imul in inc incl ins insb insd insw int into invd invlpg iret iretd iretw
ja jae jb jbe jc jcxz je jecxz jg jge jl jle jmp jna jnae jnb jnbe jnc jne jng
jnge jnl jnle jno jnp jns jnz jo jp jpe jpo js jz
lahf lar lds lea leal leave les lfs lgdt lgs lidt lldt lmsw lock lods lodsb lodsd
lodsw loop loope loopne loopnz loopz lsl lss ltr
mov movb movd movl movq movs movsb movsd movsw movsx movw movzx movzwl mul
neg nop not or out outs outsb outsd outsw
packssdw packsswb packuswb paddb paddd paddsb paddsw paddusb paddusw paddw pand
pandn pavgusb pcmpeqb pcmpeqd pcmpeqw pcmpgtb pcmpgtd pcmpgtw pf2id pfacc pfadd
pfcmpeq pfcmpge pfcmpgt pfmax pfmin pfmul pfrcp pfrcpit1 pfrcpit2 pfrsqit1
pfrsqrt pfsub pfsubr pi2fd pmaddwd pmulhrw pmulhw pmullw pop popa popad popaw
popf popfd popfw popl por prefetch prefetchw pslld psllq psllw psrad psraw psrld
psrlq psrlw psubb psubd psubsb psubsw psubusb psubusw psubw punpckhbw punpckhdq
punpckhwd punpcklbw punpckldq punpcklwd push pusha pushad pushaw pushf pushfd
pushfw pushl pxor
rcl rcr rep repe repne repnz repz ret rol ror
sahf sal sar sbb scas scasb scasd scasw seta setae setb setbe setc sete setg
setge setl setle setna setnae setnb setnbe setnc setne setng setnge setnl setnle
setno setnp setns setnz seto setp setpo sets setz sgdt shl shld shll shr shrd shrl sidt
sldt smsw stc std sti stos stosb stosd stosw str sub subl
test testl
verr verw
wait wbinvd
xadd xchg xlat xlatb xor xorl
- User set 1:
bbs
code
data
text
- User set 2:
ax bx cx dx ex si di bp sp ss es ds cs ip
al ah bl bh ch cl dh dl eh el
eax ebx ebp ecx edi edx esi esp
xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7
- User set 3:
absolute align alignb at
bits
common
dword
end endstruc extern externdef
file
globl global globaldef
istruc iend
long
org
qword
short section segment segment_data segment_code size string struc
times type tword
word
- User set 4:
assign
byte
db dd define dq dt dw
else endif endmacro endrep equ error
if ifdef incbin include ifctx ifid ifidn ifidni ifnum ifstr
macro
undef
repl resb resw resq rotate
seg
wrt
