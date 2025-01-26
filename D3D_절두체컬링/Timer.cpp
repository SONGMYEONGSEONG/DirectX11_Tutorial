#include "Timer.h"

Timer::Timer() : isRunning(false)
{
	//high_resolution_clock : system_clock의 typedef.(틱 기간이 가장 짧은 시계)
	//high_resolution_clock::now() = clock헤더의 current 와 유사 
	start = std::chrono::high_resolution_clock::now();
	stop = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
}

//델타타임을 밀리초로 호출하는 get함수 
double Timer::GetMilisecondsElapesed()
{

	//duration은 두 시점사이의 시간간격(interval)을 표현하는 클래스 템플릿
	//duration은 +, -, *, /, %, ++, --, +=, -=, *=, /=, %=와 같은 산술 연산과 비교 연산(==, <=>)을 제공
	//틱(tick)과 틱 주기(tick period)에 대한 값을 저장  , 틱주기(tick period) : 두 틱 사이의 초 단위 간격
	std::chrono::duration<double, std::milli> elapsed;
	//틱 주기가 1밀리초인 duration 코드 
	if (isRunning)
	{
		elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
		//duration::count() -> 시간 간격의 클록 틱 수를 반환
		return elapsed.count();
	}
	elapsed = std::chrono::duration<double, std::milli>(stop - start);
	return elapsed.count();
}

bool Timer::Start()
{
	if (isRunning) return false;

	start = std::chrono::high_resolution_clock::now();
	isRunning = true;
	return true;
}

bool Timer::Stop()
{
	if (!isRunning) return false;

	stop = std::chrono::high_resolution_clock::now();
	isRunning = false;
	return true;
}

void Timer::Restart()
{
	start = std::chrono::high_resolution_clock::now();
	isRunning = true;
}
