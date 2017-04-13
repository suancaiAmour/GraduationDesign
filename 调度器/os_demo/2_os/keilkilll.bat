del *.bak /s
del *.ddk /s
del *.edk /s
del *.lst /s

::del *.lnp /s

del *.mpf /s
del *.mpj /s
del *.obj /s
del *.omf /s

::del *.opt /s  ::不允许删除JLINK的设置

del *.plg /s
del *.rpt /s
del *.tmp /s

::del *.__i /s :: 对于使用命令行生成项目的时候有用

del *.crf /s
del *.o /s
del *.d /s

::del *.axf /s

del *.tra /s
del *.dep /s           
del JLinkLog.txt /s

del *.iex /s

:: htm 是注释文档
::del *.htm /s

::del *.sct /s
del *.map /s

::Visual Studio 的临时文件
del *.sdf /s

::输出的目标文件
del *.hex /s