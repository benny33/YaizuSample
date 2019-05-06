#include "dataaccess.h"
#include "../../../YaizuComLib/src/stkpl/StkPl.h"
#include "../../../YaizuComLib/src/commonfunc/StkStringParser.h"
#include "ApiGetCommandForOperation.h"

StkObject* ApiGetCommandForOperation::Execute(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3])
{
	wchar_t TargetAgtName[DA_MAXLEN_OF_AGTNAME];
	StkStringParser::ParseInto1Param(UrlPath, L"/api/opcommand/$/", L'$', TargetAgtName, DA_MAXLEN_OF_AGTNAME);
	StkObject* TmpObj = new StkObject(L"");
	while (true) {
		StkPlSleepMs(1000);

		int PInterval = 0;
		int SaInterval = 0;
		DataAccess::GetInstance()->GetServerInfo(&PInterval, &SaInterval);
		if (PInterval <= 0) {
			PInterval = 30;
		}

		int Year = 0;
		int Mon = 0;
		int Day = 0;
		int Hour = 0;
		int Min = 0;
		int Sec = 0;
		StkPlGetTime(&Year, &Mon, &Day, &Hour, &Min, &Sec, false);

		int OpCmd = DataAccess::GetInstance()->GetAgentInfoForOpCmd(TargetAgtName);
		if (OpCmd != -1) {
			int Id[DA_MAXNUM_OF_CMDRECORDS];
			wchar_t Name[DA_MAXNUM_OF_CMDRECORDS][DA_MAXLEN_OF_CMDNAME];
			int Type[DA_MAXNUM_OF_CMDRECORDS];
			char Script[DA_MAXNUM_OF_CMDRECORDS][DA_MAXLEN_OF_CMDSCRIPT];
			int ResCmdCount = DataAccess::GetInstance()->GetCommand(Id, Name, Type, Script);

			for (int Loop = 0; Loop < ResCmdCount; Loop++) {
				if (OpCmd == Id[Loop]) {
					wchar_t WScript[DA_MAXLEN_OF_CMDSCRIPT / 2] = L"";
					wchar_t* Ptr = WScript;
					for (wchar_t* LoopPtr = (wchar_t*)Script[Loop]; *LoopPtr != L'\0'; LoopPtr++) {
						if (*LoopPtr == L'\n') {
							if (Type[Loop] == 1) {
								*Ptr = L'\r';
								*(Ptr + 1) = L'\n';
								Ptr += 2;
							} else {
								*Ptr = *LoopPtr;
								Ptr++;
							}
						} else {
							*Ptr = *LoopPtr;
							Ptr++;
						}
						if ((Ptr - (wchar_t*)Script[Loop]) == DA_MAXLEN_OF_CMDSCRIPT / 2) {
							break;
						}
					}
					*Ptr = L'\0';

					TmpObj->AppendChildElement(new StkObject(L"Msg0", L"Execution"));
					StkObject* CommandObj = new StkObject(L"Command");
					CommandObj->AppendChildElement(new StkObject(L"Id", Id[Loop]));
					CommandObj->AppendChildElement(new StkObject(L"Name", Name[Loop]));
					CommandObj->AppendChildElement(new StkObject(L"Type", Type[Loop]));
					CommandObj->AppendChildElement(new StkObject(L"Script", WScript));
					TmpObj->AppendChildElement(CommandObj);
					*ResultCode = 200;
					break;
				}
			}
			DataAccess::GetInstance()->SetAgentInfoForOpCmd(TargetAgtName, -1);
			break;
		}

		if ((PInterval == 30 && Sec % 30 == 0) ||
			(PInterval == 60 && Sec == 0) ||
			(PInterval == 300 && Min % 5 == 0 && Sec == 0) ||
			(PInterval == 900 && Min % 15 == 0 && Sec == 0)) {
			TmpObj->AppendChildElement(new StkObject(L"Msg0", L"Timeout"));
			*ResultCode = 200;
			break;
		}
	}

	return TmpObj;
}