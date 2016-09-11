#pragma once

	struct EntPlayer {
		DWORD EnemyCross;
		DWORD list;
		int EnemyHealth;
		int EnemyTeam;
	};

	struct Playerstr {
		int
			iTeam,
			iClip,
			iWepID,
			iCrossID,
			iHealth;

		bool
			bLife,
			bSpawned,
			bDormant;

		Vector pos;
	};
