//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2023 iwm-iwama"
#define   IWM_VERSION         "iwmesc_20231225"
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

	// -h | --help
	if(! $ARGC || iCLI_getOptMatch(0, L"-h", L"--help"))
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

	WS *wp1 = 0;
	WS *wp2 = 0;

	// -e | -echo
	if(iCLI_getOptMatch(0, L"-e", L"-echo"))
	{
		WS *wa1[] = { L"-e", L"-echo" };
		wp1 = iws_cnv_escape(rtnArgPointer(wa1));
			P1W(wp1);
		ifree(wp1);
	}
	// -c | -command
	else if(iCLI_getOptMatch(0, L"-c", L"-command"))
	{
		WS *wa1[] = { L"-c", L"-command" };
		wp1 = rtnArgPointer(wa1);
		// STDERR 制御
		wp1 = iws_cats(2, wp1, L" 2> NUL");
			wp2 = iws_popen(wp1);
				P1W(wp2);
			ifree(wp2);
		ifree(wp1);
	}
	// -s | -script
	else if(iCLI_getOptMatch(0, L"-s", L"-script"))
	{
		WS *wa1[] = { L"-s", L"-script" };
		wp1 = rtnArgPointer(wa1);
		// STDERR 制御
		wp1 = iws_cats(2, wp1, L" 2> NUL");
			imv_system(wp1, TRUE);
		ifree(wp1);
	}

	///icalloc_mapPrint(); ifree_all(); icalloc_mapPrint();

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
	P(
		" %s\n"
		"    %s+%s\n"
		, IWM_COPYRIGHT, IWM_VERSION, LIB_IWMUTIL_VERSION
	);
	LN(80);
	P1(IESC_RESET);
}

VOID
print_help()
{
	MS *_cmd = W2M($CMD);

	print_version();
	P(
		IESC_TITLE1	" エスケープシーケンスを含むコマンドを実行／出力 "
		IESC_RESET	"\n"
		IESC_STR1	"    %s"
		IESC_OPT2	" [Option]"
		IESC_OPT1	" [Str]\n\n"
		IESC_LBL1	" (例１)\n"
		IESC_STR1	"    %s"
		IESC_OPT2	" -echo"
		IESC_OPT1	" \\033[92mテキスト\\n\\033[93m表示\\033[0m\\n\n\n"
		IESC_LBL1	" (例２)\n"
		IESC_STR1	"    %s"
		IESC_OPT2	" -command"
		IESC_OPT1	" dir /b\n\n"
		IESC_LBL1	" (例３)\n"
		IESC_STR1	"    %s"
		IESC_OPT2	" -script"
		IESC_OPT1	" python -c \"print('\\033[92mテキスト\\n\\033[93m表示\\033[0m\\n')\"\n\n"
		, _cmd, _cmd, _cmd, _cmd
	);
	P1(
		IESC_OPT2	" [Option]\n"
		IESC_OPT21	"    -echo Str | -e Str\n"
		IESC_STR1	"        Strをテキスト表示\n\n"
		IESC_OPT21	"    -command Str | -c Str\n"
		IESC_STR1	"        Strをコマンドとして実行\n"
		IESC_STR2	"        ※出力コードはコンソール基準\n\n"
		IESC_OPT21	"    -script Str | -s Str\n"
		IESC_STR1	"        Strをスクリプトとして実行\n"
		IESC_STR2	"        ※出力コードはスクリプト基準\n\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);

	ifree(_cmd);
}
