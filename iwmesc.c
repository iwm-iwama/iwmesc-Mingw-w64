//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2023-2025 iwm-iwama"
#define   IWM_FILENAME        "iwmesc"
#define   IWM_UPDATE          "20250413"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
VOID      print_version();
VOID      print_help();

INT
main()
{
	// lib_iwmutil2 初期化
	imain_begin();

	///iCLI_VarList();

	// パイプ経由 STDIN
	WS *wpStdin = iCLI_getStdin(FALSE);

	// -t | -text
	if(iCLI_getOptMatch(0, L"-t", L"-text") && *wpStdin)
	{
		WS *wp1 = iws_withoutESC(wpStdin);
			QP1W(wp1);
		ifree(wp1);
	}
	// STDIN
	else if(*wpStdin)
	{
		QP1W(wpStdin);
	}
	// -h | --help
	else if((! *wpStdin && ! $ARGC) || iCLI_getOptMatch(0, L"-h", L"--help"))
	{
		print_version();
		print_help();
	}
	// -v | --version
	else if(iCLI_getOptMatch(0, L"-v", L"--version"))
	{
		print_version();
	}
	// -s STR | -script STR
	else if(iCLI_getOptMatch(0, L"-s", L"-script") && $ARGC > 1)
	{
		WS *wpShebang = NULL;

		// ファイルが存在しないとき _wfopen() がエラーを返す
		FILE *iFp = _wfopen($ARGV[1], L"rb");
		if(iFp)
		{
			// 1行目を取得
			MS sBuf[256];
			fgets(sBuf, sizeof(sBuf), iFp);
			// UTF-8 BOM を仮変換
			memset(sBuf, '\n', iun_bomLen(sBuf));
			// 前後の '\n' 等を消去
			MS *mp1 = ims_trim(sBuf);
				// シバン行を取得
				//   #! の記述は「1行1列目」に限定する
				if(imn_len(mp1) >= 2 && mp1[0] == '#' && mp1[1] == '!')
				{
					MS *mpEnd = mp1 + imn_len(mp1) - 1;
					while(*mpEnd)
					{
						if(*mpEnd == ' ' || *mpEnd == '/' || *mpEnd == '!')
						{
							break;
						}
						--mpEnd;
					}
					++mpEnd;
					wpShebang = M2W(mpEnd);
				}
			ifree(mp1);
			fclose(iFp);
		}

		$struct_iVBW *iVBW = iVBW_alloc();
			UINT u1 = 1;
			while($ARGV[u1])
			{
				iVBW_push2(iVBW, $ARGV[u1]);
				iVBW_push2(iVBW, L" ");
				++u1;
			}

			if(wpShebang)
			{
				WS *wp1 = iws_cats(3, wpShebang, L" ", iVBW_getStr(iVBW));
					ifree(wpShebang);
					iCLI_systemW(wp1);
				ifree(wp1);
			}
			else
			{
				iCLI_systemW(iVBW_getStr(iVBW));
			}
		iVBW_freeAll(iVBW);
	}
	// else
	else
	{
		$struct_iVBW *iVBW = iVBW_alloc();
			UINT u1 = 0;
			while($ARGV[u1])
			{
				iVBW_push2(iVBW, $ARGV[u1]);
				++u1;
			}
			WS *wp1 = iws_cnv_escape(iVBW_getStr(iVBW));
				QP1W(wp1);
			ifree(wp1);
		iVBW_freeAll(iVBW);
	}

	ifree(wpStdin);

	// Debug
	///idebug_map();
	///ifree_all();
	///idebug_map();

	// 最終処理
	imain_end();
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
	P1(
		"\033[1G"	IESC_TITLE1	" エスケープシーケンスを含む文字列を表示 "	IESC_RESET	"\n"
		"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT2	" [Option]"	IESC_OPT1	" [Str]"	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例１)"	IESC_STR1	" 引数渡し"	"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT1	" \"\\033[92m\" \"テキスト\\n\" \"\\033[96m表示\" \"\\033[0m\\n\""	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例２)"	IESC_STR1	" パイプ渡し"	"\n"
		"\033[5G"	IESC_OPT1	"ls | "	IESC_STR1	IWM_FILENAME	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例３)"	IESC_STR1	" パイプ渡し／エスケープシーケンス非表示"	"\n"
		"\033[5G"	IESC_OPT1	IWM_FILENAME	" \"\\033[92mテキスト表示\\033[0m\\n\" | "	IESC_STR1	IWM_FILENAME	IESC_OPT2	" -text"	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例４)"	IESC_STR1	" -script 直接実行"	"\n"
		"\033[5G"	IWM_FILENAME	IESC_OPT2	" -script"	IESC_OPT1	" python -c \"print('\\033[92mテキスト\\n\\033[96m表示\\033[0m\\n')\""	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例５)"	IESC_STR1	" -script スクリプトファイルから実行"	"\n"
		"\033[5G"	IWM_FILENAME	IESC_OPT2	" -script "	IESC_STR2	"\033[44m"	" foo "	"\033[49m"	" ..."	"\n"
		IESC_STR2
		"\033[9G"	"\033[44m"	"                                                    "	"\033[49m"	"\n"
		"\033[9G"	"\033[44m"	"  #!/usr/bin/env python                             "	"\033[49m"	"\n"
		"\033[9G"	"\033[44m"	"  #coding:utf-8                                     "	"\033[49m"	"\n"
		"\033[9G"	"\033[44m"	"  print('\\033[92mテキスト\\n\\033[96m表示\\033[0m\\n')  "	"\033[49m"	"\n"
		"\033[9G"	"\033[44m"	"                                                    "	"\033[49m"	"\n"
		"\033[9G"	IESC_STR2	"※ ... は fooスクリプトファイルの引数 になる"	"\n"
		"\n"
		"\033[2G"	IESC_OPT2	"[Option]"	"\n"
		"\033[5G"	IESC_OPT21	"-text | -t"	"\n"
		"\033[9G"	IESC_STR1	"エスケープシーケンス非表示"	"\n"
		"\033[9G"	IESC_STR2	"※ パイプ渡し時のみ使用可能"	"\n"
		"\n"
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
