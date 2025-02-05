//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2023-2025 iwm-iwama"
#define   IWM_FILENAME        "iwmesc"
#define   IWM_UPDATE          "20250202"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
WS        *rtnArgPointerW(WS *aCmd[]);
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
		WS *wpArg = rtnArgPointerW(wa1);

		BOOL bExecuted = FALSE;

		// OK: foo／NG: "foo"
		FILE *iFp = _wfopen($ARGV[1], L"rb");
		if(iFp)
		{
			// 1行目を取得
			MS buf[256];
			MS *mp1 = fgets(buf, sizeof(buf), iFp);

			MS *mpEnd = 0;

			// 行末 '\n' '\r' 消去
			mpEnd = mp1 + imn_len(mp1) - 1;
			while(*mpEnd)
			{
				if(*mpEnd == '\n' || *mpEnd == '\r')
				{
					*mpEnd = '\0';
				}
				else
				{
					break;
				}
				--mpEnd;
			}

			// UTF-8 BOM
			if(imn_len(mp1) >= 3 && mp1[0] == (MS)0xEF && mp1[1] == (MS)0xBB && mp1[2] == (MS)0xBF)
			{
				mp1 += 3;
			}

			// シバン行を取得
			//   #! の記述は「1行目1列目」に限定する
			if(imn_len(mp1) >= 2 && mp1[0] == '#' && mp1[1] == '!')
			{
				bExecuted = TRUE;

				mpEnd = mp1 + imn_len(mp1) - 1;
				while(*mpEnd)
				{
					if(*mpEnd == ' ' || *mpEnd == '/' || *mpEnd == '!')
					{
						break;
					}
					--mpEnd;
				}
				++mpEnd;
				WS *wp1 = M2W(mpEnd);
					WS *wp2 = iws_cats(3, wp1, L" ", wpArg);
						imv_systemW(wp2);
					ifree(wp2);
				ifree(wp1);
			}
			fclose(iFp);
		}

		if(! bExecuted)
		{
			imv_systemW(wpArg);
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
*rtnArgPointerW(
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
		"\033[2G"	IWM_COPYRIGHT	"\n"
		"\033[5G"	IWM_FILENAME	"_"	IWM_UPDATE	" + "	LIB_IWMUTIL_FILENAME	"\n"
	);
	LN(80);
	P1(IESC_RESET);
}

VOID
print_help()
{
	print_version();
	P1(
		"\033[1G"	IESC_TITLE1	" エスケープシーケンスを含むコマンドを実行／出力 "	IESC_RESET	"\n"
		"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT2	" [Option]"	IESC_OPT1	" [Str]"	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例１)"	IESC_STR1	" 引数渡し"	"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT1	" \"\\033[92m\" \"テキスト\\n\" \"\\033[96m表示\" \"\\033[0m\\n\""	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例２)"	IESC_STR1	" パイプ渡し"	"\n"
		"\033[5G"	IESC_OPT1	"ls | "	IESC_STR1	IWM_FILENAME	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例３)"	IESC_STR1	" -script 直接実行"	"\n"
		"\033[5G"	IWM_FILENAME	IESC_OPT2	" -script"	IESC_OPT1	" python -c \"print('\\033[92mテキスト\\n\\033[96m表示\\033[0m\\n')\""	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例４)"	IESC_STR1	" -script スクリプトファイルから実行"	"\n"
		"\033[5G"	IWM_FILENAME	IESC_OPT2	" -script "	IESC_STR2	"\033[44m"	" foo "	"\033[49m"	" ..."	"\n"
		IESC_STR2
		"\033[9G"	"\033[44m"	"                                                    "	"\033[49m"	"\n"
		"\033[9G"	"\033[44m"	"  #!/usr/bin/env python                             "	"\033[49m"	"\n"
		"\033[9G"	"\033[44m"	"  #coding:utf-8                                     "	"\033[49m"	"\n"
		"\033[9G"	"\033[44m"	"  print('\\033[92mテキスト\\n\\033[96m表示\\033[0m\\n')  "	"\033[49m"	"\n"
		"\033[9G"	"\033[44m"	"                                                    "	"\033[49m"	"\n"
		"\033[9G"	IESC_STR1	"※ ... は fooスクリプトファイルの引数 になる"	"\n"
		"\n"
		"\033[2G"	IESC_OPT2	"[Option]"	"\n"
		"\033[5G"	IESC_OPT21	"-script Str | -s Str"	"\n"
		"\033[9G"	IESC_STR1	"Strをスクリプトとして実行"	"\n"
		"\033[9G"	IESC_STR1	"Strがスクリプトファイルのとき シバン行(*1)のインタプリタを使用"	"\n"
		"\033[9G"	IESC_STR2	"(*1) shebang／#!から始まる１行目"	"\n"
		"\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);
}
