#include "surakarta/surakarta_agent/surakarta_agent_mine.h"
#include <algorithm>
#include <cstdlib>
#include <random>
#include <vector>
#include "surakarta/surakarta_common.h"

SurakartaMove SurakartaAgentMine::CalculateMove() {
    // TODO: Implement your own ai here.
    // 生成from集与to集
    std::vector<SurakartaPosition> from;
    std::vector<SurakartaPosition> to;
    std::vector<SurakartaPosition> opponent;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
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
    }
    //  优先选择能吃子且不被吃的MOVE，其次选能吃子但会被吃的MOVE，再次选不能吃子但不会被吃的MOVE，最后选不能吃子且会被吃的MOVE <-优先级
    SurakartaMove rd_move2({0, 0}, {0, 0}, game_info_->current_player_);
    int capflag_of_being_captured = 0, nonflag_of_not_captured = 0;
    for (auto& my_from : from) {  // 枚举我方可移动棋子
        for (auto& my_to : to) {  // 枚举当前被枚举我方子的去处
            SurakartaMove move = {p1, p2, game_info_->current_player_};
            SurakartaIllegalMoveReason reason = rule_manager_->JudgeMove(move);
            if (reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {  // 当前被枚举我方子有吃掉对方子的机会
                for (auto& p : opponent) {                                   // 枚举对方子，检查我方子移动后是否会被对方子吃掉
                    SurakartaMove move_of_opponent = {p,
                                                      p2,
                                                      (*board_)[p.x][p.y]->GetColor()};
                    PieceColor origin = (*board_)[p2.x][p2.y]->GetColor();
                    PieceColor origin_from = (*board_)[p.x][p.y]->GetColor();  // 改变原来棋盘，便于judgemove
                    (*board_)[p2.x][p2.y]->SetColor(game_info_->current_player_);
                    (*board_)[p.x][p.y]->SetColor(PieceColor::NONE);
                    SurakartaIllegalMoveReason reason_of_opponent = rule_manager_->JudgeMove(move_of_opponent);
                    (*board_)[p2.x][p2.y]->SetColor(origin);
                    (*board_)[p.x][p.y]->SetColor(origin_from);                                  // 恢复棋盘
                    if (reason_of_opponent != SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {  // 可吃对方子且不会被吃
                        return move;
                    }
                }
                if (capflag_of_being_captured == 0) {  // 记录第一个可吃对方子且会被吃掉的情况，不再记录之后相同的情况
                    rd_move2 = move;
                    capflag_of_being_captured = 1;
                }
            } else if (reason == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE) {  // 当前被枚举我方子没有吃掉对方子的机会
                for (auto& p : opponent) {                                              // 判断移动后是否会被对方子吃掉
                    SurakartaMove move_of_opponent = {p,
                                                      p2,
                                                      (*board_)[p.x][p.y]->GetColor()};
                    PieceColor origin = (*board_)[p2.x][p2.y]->GetColor();
                    PieceColor origin_from = (*board_)[p.x][p.y]->GetColor();
                    (*board_)[p2.x][p2.y]->SetColor(game_info_->current_player_);
                    (*board_)[p.x][p.y]->SetColor(PieceColor::NONE);
                    SurakartaIllegalMoveReason reason_of_opponent = rule_manager_->JudgeMove(move_of_opponent);
                    (*board_)[p2.x][p2.y]->SetColor(origin);
                    (*board_)[p.x][p.y]->SetColor(origin_from);
                    if (reason_of_opponent != SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE && capflag_of_being_captured == 0 && nonflag_of_not_captured == 0) {  // 记录第一个不会吃掉对方子且移动后不会被对方子吃掉的情况，不再记录之后相同的情况，且优先级高于此种情况的情况出现后也不再记录此种情况
                        rd_move2 = move;
                        nonflag_of_not_captured = 1;
                    }
                }
                if (capflag_of_being_captured == 0 && nonflag_of_not_captured == 0) {  // 优先级高于此种情况（移动不会吃掉对方子且会被对方子吃掉，出现概率极低但确实会出现）的情况出现后，不再记录此种情况
                    rd_move2 = move;
                }
            }
        }
    }
    return rd_move2;
}