WINDOW *GrabWindow();
int GrabDigit();
void HelpOut();
void DropWindow();
void ExtraCheck();
void GetString();
void PlayGame();
void DisplayDice();
int ComputerHold();
void DisplayDie();
void DieCentre();
void PutNumber();
int ComputerSelect();
void Eval();
void FindHold();
void InitCosts();
int DetermineWinner();
void AdjustCosts();
void DumpCosts();
void Rools();
void WriteScore();
void PopError();
void DisplayScore();
void DumpScore();
#if defined(BSD_CURSES) || defined(SYS5_2_CURSES)
int BSDbox();
#endif
