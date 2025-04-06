#!/usr/bin/env python
#coding:utf-8
import datetime
print(
	"\033[93m" +
	datetime.datetime.now().strftime("%Y/%m/%d %H:%M:%S") +
	"\033[0m"
)
print("\033[92mテキスト\n\033[96m表示\033[0m")
