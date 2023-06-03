#pragma once
#ifndef GEAR_VERSION
#define GEAR_VERSION	0xEE010305	//	EE1.3.5
/*
	Gear version format:
		S - special indicators(optional), H - major, L - minor, B - build
		0x(SS)HHLLBB
	Special indicators[SS]:
		EE - intermediate, "raw" version (should not be used by the user until completed)
*/

extern const char GearVersionString[];

#endif