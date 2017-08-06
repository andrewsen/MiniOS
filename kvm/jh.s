
jh.elf:     формат файла elf32-i386


Дизассемблирование раздела .text:

00000000 <jitCallHelper>:
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	56                   	push   %esi
   4:	57                   	push   %edi
   5:	ff 75 0c             	pushl  0xc(%ebp)
   8:	e8 fc ff ff ff       	call   9 <jitCallHelper+0x9>
   d:	83 c4 04             	add    $0x4,%esp
  10:	83 f8 00             	cmp    $0x0,%eax
  13:	74 06                	je     1b <WAIT_LOOP>
  15:	5f                   	pop    %edi
  16:	5e                   	pop    %esi
  17:	89 ec                	mov    %ebp,%esp
  19:	5d                   	pop    %ebp
  1a:	c3                   	ret    

0000001b <WAIT_LOOP>:
  1b:	ff 75 08             	pushl  0x8(%ebp)
  1e:	e8 fc ff ff ff       	call   1f <WAIT_LOOP+0x4>
  23:	83 c4 04             	add    $0x4,%esp
  26:	83 f8 00             	cmp    $0x0,%eax
  29:	74 f0                	je     1b <WAIT_LOOP>
  2b:	5f                   	pop    %edi
  2c:	5e                   	pop    %esi
  2d:	89 ec                	mov    %ebp,%esp
  2f:	5d                   	pop    %ebp
  30:	c3                   	ret    
