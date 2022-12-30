# Tetris

ncurses library를 이용한 tetris입니다.
<br>
<br>

1. main

<img src="https://user-images.githubusercontent.com/89072013/210091402-1c8c1fe0-ed8e-4df4-b6ba-49e3c030d18f.png" alt="tetris_main">

- 메인 화면

게임 플레이, 랭크 조회, 추천 시스템 기반 자동 플레이, 게임 종료의 네가지 옵션을 가지고 있는 메인 화면입니다.

`key down`에 반응해 동작합니다.

<br>
<br>

2. 게임플레이 - 수동

<img src="https://user-images.githubusercontent.com/89072013/210091650-e732f939-29db-4442-b7e9-1e59426d5934.png">

- 키보드 방향키로 조작합니다.
- R표시가 된 블록은 점수 획득 방식을 기반으로 계산된 블록을 놓을 수 있는 최적의 위치입니다.
- /표시가 된 블록은 블록이 떨어졌을 때 위치하게 되는 위치입니다.
- 다음, 그리고 그 다음 블록까지 보여집니다.
- 점수계산:
    - 해당 블록이 땅(쌓여있는 블록도 땅으로 침)과 닿는 면적
    - (line deleted)^10 * 10
<br>
<br>


3. 게임플레이 - 자동

<img src="https://user-images.githubusercontent.com/89072013/210093289-6ebc9d28-7b3f-4b52-a513-45aeb77bb1f9.png">

- `q`를 제외한 `key down`에 반응하지 않습니다.
- 계산된 최적의 블록 위치에 해당 블록을 놓습니다.
- 구현한 추천 시스템의 평가 지표가 우측에 표시됩니다.