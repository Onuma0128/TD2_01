#pragma once
class GameState
{
public:
    static GameState& getInstance() {
        static GameState instance;
        return instance;
    }

    // Wave関連情報の保存と取得
    void setCurrentWave(int wave) { currentWave = wave; }
    int getCurrentWave() const { return currentWave; }

private:
    // 現在のWave番号
    int currentWave = 0;

    GameState() {}  // シングルトンのためのコンストラクタをプライベートにする
    GameState(const GameState&) = delete;
    void operator=(const GameState&) = delete;
};

