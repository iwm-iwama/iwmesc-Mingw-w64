【実行に必要なファイル】

	iwmesc.exe

【このプログラムについて】

	DOSプロンプトでエスケープシーケンス表示（\033[...）を行うコマンドです。

	(例１) -echo 引数渡し
		iwmesc.exe -echo \033[92mテキスト\n\033[96m表示\033[0m\n

	(例２) -echo パイプ渡し
		ls -la | iwmesc.exe -echo

	(例３) -script 直接実行
		iwmesc.exe -script python -c "print('\033[92mテキスト\n\033[96m表示\033[0m\n')"

	(例４) -script ファイルから実行
		iwmesc.exe -script foo
		                    ↑
		                   #!python
		                   #coding:utf-8
		                   print('\033[92mテキスト\n\033[96m表示\033[0m\n')
	   
