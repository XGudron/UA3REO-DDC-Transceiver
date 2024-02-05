"c:\Program Files (x86)\Windows Kits\10\bin\x86\Inf2Cat.exe" /v /driver:. /os:6_3_X86,6_3_X64,8_X64,8_X86,Server8_X64,Server2008R2_X64,7_X64,7_X86,Server2008_X64,Server2008_X86,Vista_X64,Vista_X86,Server2003_X64,Server2003_X86,XP_X64,XP_X86,2000,10_X86,10_X64,Server10_X64
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x86\signtool" sign /v /n "UA3REO" /a /tr http://timestamp.globalsign.com/?signature=sha2 /td sha256 *.cat
pause
