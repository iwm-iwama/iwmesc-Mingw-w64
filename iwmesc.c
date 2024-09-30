//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2023-2024 iwm-iwama"
#define   IWM_FILENAME        "iwmesc"
#define   IWM_UPDATE          "20241001"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
WS        *rtnArgPointer(WS *aCmd[]);
VOID      print_version();
VOID      print_help();

INT
main()
{
	// lib_iwmutil2 初期化
	imain_begin();

	///iCLI_VarList();

	// パイプ経由 STDIN
	WS *Stdin = iCLI_GetStdin(FALSE);
	BOOL bStdin = (*Stdin ? TRUE : FALSE);

	// -h | --help
	if((! bStdin && ! $ARGC) || iCLI_getOptMatch(0, L"-h", L"--help"))
	{
		print_help();
		imain_end();
	}

	// -v | --version
	if(iCLI_getOptMatch(0, L"-v", L"--version"))
	{
		print_version();
		imain_end();
	}

	if(bStdin)
	{
		P1W(Stdin);
	}

	// -s STR | -script STR
	if(! bStdin && iCLI_getOptMatch(0, L"-s", L"-script"))
	{
		WS *wa1[] = { L"-s", L"-script" };
		WS *Arg = rtnArgPointer(wa1);

		BOOL bExecuted = FALSE;

		// OK: foo／NG: "foo"
		FILE *iFp = _wfopen($ARGV[1], L"rb");
		if(iFp)
		{
			// 1行目を取得
			//   #! の記述は「1行目」に限定する
			MS buf[256];
			MS *mp1 = fgets(buf, sizeof(buf), iFp);
			INT iLen = imn_len(mp1);

			// UTF-8 BOM
			if(iLen >= 3 && mp1[0] == (MS)0xEF && mp1[1] == (MS)0xBB && mp1[2] == (MS)0xBF)
			{
				mp1 += 3;
				iLen -= 3;
			}

			// 先頭の空白をスルー
			while(iLen >= 0 && (*mp1 == ' ' || *mp1 == '\t'))
			{
				++mp1;
				--iLen;
			}

			// #! インタプリタ名を取得
			if(iLen >= 2 && mp1[0] == '#' && mp1[1] == '!')
			{
				// strlen("#!") => 2
				mp1 += 2;
				bExecuted = TRUE;

				WS *wp1 = M2W(mp1);
					WS *wp2 = iws_trimR(wp1);
						// /usr/bin/env を無効化
						WS *wp3 = iws_replace(wp2, L"/usr/bin/env ", L"", FALSE);
						// /usr/bin/ を無効化
						WS *wp4 = iws_replace(wp3, L"/usr/bin/", L"", FALSE);
							WS *wp5 = iws_cats(3, wp4, L" ", Arg);
								imv_systemW(wp5);
							ifree(wp5);
						ifree(wp4);
						ifree(wp3);
					ifree(wp2);
				ifree(wp1);
			}
			fclose(iFp);
		}

		if(! bExecuted)
		{
			imv_systemW(Arg);
		}
	}
	else
	{
		for(UINT _u1 = 0; _u1 < $ARGC; _u1++)
		{
			WS *wp1 = iws_cnv_escape($ARGV[_u1]);
				P1W(wp1);
			ifree(wp1);
		}
	}

	///idebug_map(); ifree_all(); idebug_map();

	// 最終処理
	imain_end();
}

WS
*rtnArgPointer(
	WS *aCmd[]
)
{
	WS *rtn = 0;
	for(UINT _u1 = 0; _u1 < 2; _u1++)
	{
		WS *wp1 = iws_cats(2, aCmd[_u1], L" ");
		if(iwb_cmpf($ARG, wp1))
		{
			rtn = ($ARG + wcslen(wp1));
			ifree(wp1);
			break;
		}
		ifree(wp1);
	}
	return rtn;
}

VOID
print_version()
{
	P1(IESC_STR2);
	LN(80);
	P1(
		" " IWM_COPYRIGHT "\n"
		"    " IWM_FILENAME "_" IWM_UPDATE " + " LIB_IWMUTIL_FILENAME "\n"
	);
	LN(80);
	P1(IESC_RESET);
}

VOID
print_help()
{
	print_version();
	P1(
		IESC_TITLE1	" エスケープシーケンスを含むコマンドを実行／出力 " IESC_RESET "\n\n"
		IESC_STR1	"    " IWM_FILENAME
		IESC_OPT2	" [Option]"
		IESC_OPT1	" [Str]\n\n\n"
		IESC_LBL1	" (例１)"
		IESC_STR1	" 引数渡し\n"
					"    " IWM_FILENAME
		IESC_OPT1	" \"\\033[92m\" \"テキスト\\n\" \"\\033[96m表示\" \"\\033[0m\\n\"\n\n"
		IESC_LBL1	" (例２)"
		IESC_STR1	" パイプ渡し\n"
		IESC_OPT1	"    ls |"
		IESC_STR1	" " IWM_FILENAME "\n\n"
		IESC_LBL1	" (例３)"
		IESC_STR1	" -script 直接実行\n"
					"    " IWM_FILENAME
		IESC_OPT2	" -script"
		IESC_OPT1	" python -c \"print('\\033[92mテキスト\\n\\033[96m表示\\033[0m\\n')\"\n\n"
		IESC_LBL1	" (例４)"
		IESC_STR1	" -script ファイルから実行\n"
					"    " IWM_FILENAME
		IESC_OPT2	" -script"
		IESC_STR1   "\033[20G\033[44m foo \033[49m\n"
					"\033[20G\033[44m                                                    \033[49m\n"
					"\033[20G\033[44m  #!/usr/bin/env python                             \033[49m\n"
					"\033[20G\033[44m  #coding:utf-8                                     \033[49m\n"
					"\033[20G\033[44m  print('\\033[92mテキスト\\n\\033[96m表示\\033[0m\\n')  \033[49m\n"
					"\033[20G\033[44m                                                    \033[49m\n"
		IESC_STR2	"\033[20G ※foo以降の引数は\"fooの引数\"になる\n\n"
	);
	P1(
		IESC_OPT2	" [Option]\n"
		IESC_OPT21	"    -script Str | -s Str\n"
		IESC_STR1	"        Strをスクリプトとして実行\n"
					"        Strがスクリプトファイルのとき（1行目）#! のインタプリタを使用\n"
		IESC_STR2	"        ※出力コードはスクリプト基準\n\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);
}
