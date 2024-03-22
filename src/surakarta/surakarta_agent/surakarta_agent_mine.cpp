#include "surakarta/surakarta_agent/surakarta_agent_mine.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <vector>
#include "surakarta/surakarta_common.h"

SurakartaMove SurakartaAgentMine::CalculateMove() {
    std::vector<SurakartaPosition> from;
    std::vector<SurakartaPosition> to;
    std::vector<SurakartaPosition> opponent;
    SurakartaMove last_move({0, 0}, {0, 0}, game_info_->current_player_);
    SurakartaMove can_eat_move({0, 0}, {0, 0}, game_info_->current_player_);
    SurakartaMove none_eat_move({0, 0}, {0, 0}, game_info_->current_player_);
    SurakartaMove bad_move({0, 0}, {0, 0}, game_info_->current_player_);
    SurakartaMove ran_move({0, 0}, {0, 0}, game_info_->current_player_);
    SurakartaMove fis_bad_move({0, 0}, {0, 0}, game_info_->current_player_);
    SurakartaMove fis_ran_move({0, 0}, {0, 0}, game_info_->current_player_);
    SurakartaMove fis_last_move({0, 0}, {0, 0}, game_info_->current_player_);
    srand((unsigned)time(NULL));
    unsigned int which_ran = 1 + (rand() % 9);  // 随机种子防止困顿
    unsigned int flag = 0;
    for (unsigned int i = 0; i < 6; i++) {
        for (unsigned int j = 0; j < 6; j++) {
            SurakartaPosition position = {i, j};
            if ((*board_)[i][j]->GetColor() == game_info_->current_player_) {
                from.push_back(position);
            } else {
                to.push_back(position);
            }
            if ((*board_)[i][j]->GetColor() != game_info_->current_player_ && (*board_)[i][j]->GetColor() != PieceColor::NONE && (*board_)[i][j]->GetColor() != PieceColor::UNKNOWN) {
                opponent.push_back(position);
            }
        }
    }  // 存一下哪些地方可以from/to/对手可以from
    for (auto& my_from : from) {
        for (auto& my_to : to) {
            SurakartaMove move = {my_from, my_to, game_info_->current_player_};
            SurakartaIllegalMoveReason reason = rule_manager_->JudgeMove(move);
            if (reason != SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE && reason != SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)
                continue;
            else if (reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {  // 当前被枚举我方子有吃掉对方子的机会
                int flag_eat_opponent = 0;
                PieceColor youCOLOR = (*board_)[my_to.x][my_to.y]->GetColor();
                PieceColor myCOLOR = (*board_)[my_from.x][my_from.y]->GetColor();    // 记录一下棋子颜色
                (*board_)[my_to.x][my_to.y]->SetColor(game_info_->current_player_);  // 吃掉的棋子to变为mycolor
                (*board_)[my_from.x][my_from.y]->SetColor(PieceColor::NONE);         // from的地方变为空
                for (auto& opponent_from : opponent) {                               // 枚举对方子，检查我方子移动后是否会被对方子吃掉
                    SurakartaMove move_opponent = {opponent_from, my_to, (*board_)[opponent_from.x][opponent_from.y]->GetColor()};
                    SurakartaIllegalMoveReason reason_opponent = rule_manager_->JudgeMove(move_opponent);
                    if (reason_opponent == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {  // 可吃对方子且不会被吃
                        flag_eat_opponent = 1;
                    }
                }
                (*board_)[my_to.x][my_to.y]->SetColor(youCOLOR);
                (*board_)[my_from.x][my_from.y]->SetColor(myCOLOR);  // 恢复棋盘
                if (flag_eat_opponent != 1)                          // 如果这个时候就直接吃了吧！
                    return move;
                can_eat_move = move;  // 记录一个可吃对方子
            } else {                  // 如果不能吃子的话...
                int flag_eat_opponent = 0;
                (*board_)[my_to.x][my_to.y]->SetColor(game_info_->current_player_);
                (*board_)[my_from.x][my_from.y]->SetColor(PieceColor::NONE);  // 先更新期盘
                for (auto& opponent_from : opponent) {                        // 判断移动后是否会被吃
                    SurakartaMove move_opponent = {opponent_from, my_to, (*board_)[opponent_from.x][opponent_from.y]->GetColor()};
                    SurakartaIllegalMoveReason reason_opponent = rule_manager_->JudgeMove(move_opponent);
                    if (reason_opponent == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {
                        flag_eat_opponent = 1;
                    }
                }
                (*board_)[my_to.x][my_to.y]->SetColor(PieceColor::NONE);
                (*board_)[my_from.x][my_from.y]->SetColor(game_info_->current_player_);
                if ((move.to.x == 0) + (move.to.y == 0) + (move.to.x == 5) + (move.to.y == 5) == 2) {
                    bad_move = move;
                    continue;
                }
                flag++;
                if (flag_eat_opponent != 1 && flag == which_ran)
                    none_eat_move = move;
                if (flag_eat_opponent != 1 && flag != which_ran)
                    ran_move = move;
                last_move = move;
            }
        }
    }
    for (auto& my_from : from) {
        for (auto& opponent_to : opponent) {
            SurakartaMove move_of_opponent = {my_from, opponent_to, (*board_)[opponent_to.x][opponent_to.y]->GetColor()};
            SurakartaIllegalMoveReason reason_opponent = rule_manager_->JudgeMove(move_of_opponent);
            if (reason_opponent == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {
                for (auto& my_to : to) {
                    {
                        int flag_eat_opponent = 0;
                        SurakartaMove move = {my_from, my_to, game_info_->current_player_};
                        SurakartaIllegalMoveReason reason = rule_manager_->JudgeMove(move);
                        if (reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {
                            for (auto& p : opponent) {
                                SurakartaMove move_of_opponent = {p, my_to, (*board_)[p.x][p.y]->GetColor()};
                                (*board_)[my_to.x][my_to.y]->SetColor(game_info_->current_player_);
                                (*board_)[my_from.x][my_from.y]->SetColor(PieceColor::NONE);
                                SurakartaIllegalMoveReason reason_of_opponent = rule_manager_->JudgeMove(move_of_opponent);
                                (*board_)[my_from.x][my_from.y]->SetColor(game_info_->current_player_);
                                (*board_)[my_to.x][my_to.y]->SetColor(PieceColor::NONE);
                                if (reason_opponent == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {  // 可吃对方子且不会被吃
                                    flag_eat_opponent = 1;
                                }
                            }
                            if (flag_eat_opponent != 1)  // 如果这个时候就直接吃了吧！
                                return move;
                        }

                        else if (reason == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE) {
                            int flag_eat_opponent = 0;
                            (*board_)[my_to.x][my_to.y]->SetColor(game_info_->current_player_);
                            (*board_)[my_from.x][my_from.y]->SetColor(PieceColor::NONE);  // 先更新期盘
                            for (auto& opponent_from : opponent) {                        // 判断移动后是否会被吃
                                SurakartaMove move_opponent2 = {opponent_from, my_to, (*board_)[opponent_from.x][opponent_from.y]->GetColor()};
                                SurakartaIllegalMoveReason reason_opponent = rule_manager_->JudgeMove(move_opponent2);
                                if (reason_opponent == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {
                                    flag_eat_opponent = 1;
                                }
                            }
                            (*board_)[my_to.x][my_to.y]->SetColor(PieceColor::NONE);
                            (*board_)[my_from.x][my_from.y]->SetColor(game_info_->current_player_);
                            if ((move.to.x == 0) + (move.to.y == 0) + (move.to.x == 5) + (move.to.y == 5) == 2) {
                                fis_bad_move = move;
                                continue;
                            }
                            flag++;
                            if (flag_eat_opponent != 1 && flag == which_ran)
                                none_eat_move = move;
                            if (flag_eat_opponent != 1 && flag != which_ran)
                                fis_ran_move = move;
                            fis_last_move = move;
                        }
                    }
                }
            }
        }
    }
    SurakartaIllegalMoveReason reason1 = rule_manager_->JudgeMove(can_eat_move);
    SurakartaIllegalMoveReason reason2 = rule_manager_->JudgeMove(none_eat_move);
    SurakartaIllegalMoveReason reason3 = rule_manager_->JudgeMove(bad_move);
    SurakartaIllegalMoveReason reason4 = rule_manager_->JudgeMove(ran_move);
    SurakartaIllegalMoveReason reason5 = rule_manager_->JudgeMove(fis_ran_move);
    SurakartaIllegalMoveReason reason6 = rule_manager_->JudgeMove(fis_last_move);
    SurakartaIllegalMoveReason reason7 = rule_manager_->JudgeMove(fis_bad_move);
    if (reason1 == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE)  // 优先级，能吃的先走
        return can_eat_move;

    if (reason5 == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)  // 不能吃的，但走了也不会被吃的
        return fis_ran_move;
    if (reason7 == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)  // 最差的走法
        return fis_bad_move;
    if (reason6 == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)  // 不能吃的，但走了也不会被吃的（随机版本）
        return fis_last_move;
    if (reason2 == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)  // 不能吃的，但走了也不会被吃的（随机版本）
        return none_eat_move;
    if (reason4 == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)  // 不能吃的，但走了也不会被吃的
        return ran_move;
    if (reason3 == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)  // 最差的走法
        return bad_move;
    return last_move;  // 走投无路的走法
}