SET PATH=D:\Program Files\Keil\ARM\BIN40;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\Intel\DMIX;D:\Program Files\Tencent\QQPCMgr\6.8.2395.401;C:\Program Files\Microsoft SQL Server\100\Tools\Binn\;C:\Program Files\Microsoft SQL Server\100\DTS\Binn\;D:\Program Files\MATLAB\R2010b\runtime\win32;D:\Program Files\MATLAB\R2010b\bin;C:\Program Files\Intel\WiFi\bin\;C:\Program Files\Common Files\Intel\WirelessCommon\;D:\Program Files\Tencent\QQPCMgr\6.8.2395.401;D:\Program Files\Android\android-sdk\tools;D:\Program Files\Android\android-sdk\platform-tools;D:\java\bin
SET ARMCC41INC=D:\Program Files\Keil\ARM\RV31\Inc
SET ARMCC41LIB=D:\Program Files\Keil\ARM\RV31\Lib
SET CPU_TYPE=STM32F103RB
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=APP
SET CPU_CLOCK=0x007A1200
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\cpp_test.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\stm32f10x__tim.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\stm32f10x__spi.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\allocator.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\sys.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\stm32f10x_it.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\usart.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\iostream.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\type.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\utilities.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\debug.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmAsm" --Via ".\output\startup_stm32f10x_md._ia"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\core_cm3.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\stm32f10x__gpio_cpin.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\stm32f10x__nvic.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\stm32f10x__rcc.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\stm32f10x__usart.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\spi.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\fat_fs.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\mmc_sd.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\ili93xx.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\lcd_gui.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmCC" --Via ".\output\font.__i"
"D:\Program Files\Keil\ARM\BIN40\ArmLink" --Via ".\output\stm32test.lnp"
"D:\Program Files\Keil\ARM\BIN40\fromelf.exe" ".\output\stm32test.axf" --i32combined --output ".\output\stm32test.hex"
