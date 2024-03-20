#include "surakarta_rule_manager.h"
#include <iostream>
void SurakartaRuleManager::OnUpdateBoard() {}
int weizhi_line_1(unsigned int i, unsigned int j) {
    if (i == 1)
        return j;
    if (j == 4)
        return i + 6;
    if (i == 4)
        return 17 - j;

    return 23 - i;
}  // 在第一条吃子链条上时，根据坐标给该位置标序号
int weizhi_line_2(unsigned int i, unsigned int j) {
    if (i == 2)
        return j;
    if (j == 3)
        return i + 6;
    if (i == 3)
        return 17 - j;

    return 23 - i;
}  // 在第二条吃子链条上时，根据坐标给该位置标序号
int which_line(unsigned int i, unsigned int j) {
    int f1 = 0, f2 = 0;
    if ((i == 1) || (i == 4) || (j == 1) || (j == 4))
        f1 = 1;
    if ((i == 2) || (i == 3) || (j == 2) || (j == 3))
        f2 = 2;
    return f1 + f2;
}  // 判断在第几条吃子链条上/可以是1/2/3（3代表两个都有）
int PO2_1(int po_to) {
    if (po_to == 4 || po_to == 10 || po_to == 16)
        return po_to + 3;
    if (po_to == 1)
        return 22;
    return 1000;
}
SurakartaIllegalMoveReason SurakartaRuleManager::JudgeMove(const SurakartaMove& move) {
    SurakartaPlayer current_player = game_info_->current_player_;
    if (move.player != game_info_->current_player_) {
        return SurakartaIllegalMoveReason::NOT_PLAYER_TURN;
    }
    if (move.from.x >= board_size_ || move.from.y >= board_size_ || move.to.x >= board_size_ || move.to.y >= board_size_) {
        return SurakartaIllegalMoveReason::OUT_OF_BOARD;  // 返回超出棋盘范围的非法移动原因
    }

    if ((*board_)[move.from.x][move.from.y]->GetColor() != PieceColor::BLACK && (*board_)[move.from.x][move.from.y]->GetColor() != PieceColor::WHITE) {
        return SurakartaIllegalMoveReason::NOT_PIECE;  // 返回移动了一个不是棋子的位置的非法移动原因
    }

    if (((*board_)[move.from.x][move.from.y]->GetColor() == PieceColor::BLACK && current_player == SurakartaPlayer::WHITE) || (((*board_)[move.from.x][move.from.y]->GetColor() == PieceColor::WHITE) && current_player == SurakartaPlayer::BLACK)) {
        return SurakartaIllegalMoveReason::NOT_PLAYER_PIECE;  // 返回移动了not the player's的非法移动原因
    }
    if ((*board_)[move.to.x][move.to.y]->GetColor() != PieceColor::BLACK && (*board_)[move.to.x][move.to.y]->GetColor() != PieceColor::WHITE) {
        int xf, yf;
        if (move.from.x <= move.to.x)
            xf = move.to.x - move.from.x;
        else
            xf = move.from.x - move.to.x;
        if (move.from.y <= move.to.y)
            yf = move.to.y - move.from.y;
        else
            yf = move.from.y - move.to.y;
        if (xf <= 1 && yf <= 1)                                         //&& (move.to.x - move.from.x) >= -1 && (move.to.y - move.from.y) <= 1 && (move.to.y - move.from.y) >= -1)
            return SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;  // 判断是否是在周围一格内吃子
        else
            return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;  // 不是在周围一格内吃子
    }
    if (((*board_)[move.to.x][move.to.y]->GetColor() == PieceColor::BLACK && current_player == SurakartaPlayer::BLACK) || (((*board_)[move.to.x][move.to.y]->GetColor() == PieceColor::WHITE) && current_player == SurakartaPlayer::WHITE)) {
        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;  // eat the same color
    }
    if (((*board_)[move.to.x][move.to.y]->GetColor() == PieceColor::BLACK || (*board_)[move.to.x][move.to.y]->GetColor() == PieceColor::WHITE) && (((move.to.x == 0) + (move.to.y == 0) + (move.to.x == board_size_ - 1) + (move.to.y == board_size_ - 1) == 2) || ((move.from.x == 0) + (move.from.y == 0) + (move.from.x == board_size_ - 1) + (move.from.y == board_size_ - 1) == 2))) {
        return SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;  // 特判断 在四个角的位置是吃不到的
    }

    int po_from, po_to, line_from, line_to;
    line_from = which_line(move.from.x, move.from.y);
    line_to = which_line(move.to.x, move.to.y);  // 判断from棋子和to棋子在which——line上
    if (line_from != 3 && line_from != line_to && line_to != 3) {
        return SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;  // 如果两个棋子不在同一条吃子路径上，肯定没救了
    }
    int line_1[1000] = {0}, line_2[1000] = {0};  // 数组存放改路径上有or无子 柚子==1，无子==0
    if (line_from == 1 || line_to == 1 || (line_to == 3 && line_from == 3)) {
        for (unsigned int i = 0; i <= 5; i++) {
            if ((*board_)[1][i]->GetColor() == PieceColor::BLACK || (*board_)[1][i]->GetColor() == PieceColor::WHITE) {
                line_1[i] = 1;
            }
            if ((*board_)[i][4]->GetColor() == PieceColor::BLACK || (*board_)[i][4]->GetColor() == PieceColor::WHITE) {
                line_1[i + 6] = 1;
            }
            if ((*board_)[4][5 - i]->GetColor() == PieceColor::BLACK || (*board_)[4][5 - i]->GetColor() == PieceColor::WHITE) {
                line_1[i + 12] = 1;
            }
            if ((*board_)[5 - i][1]->GetColor() == PieceColor::BLACK || (*board_)[5 - i][1]->GetColor() == PieceColor::WHITE) {
                line_1[i + 18] = 1;
            }
        }
        po_from = weizhi_line_1(move.from.x, move.from.y);
        po_to = weizhi_line_1(move.to.x, move.to.y);
        int po_to2 = PO2_1(po_to), po_from2 = PO2_1(po_from);  // 再交差点时候，可能会有两个序号，但是实际上只存了一格，所以需要存存po2. // 对于要去的地方其实也有交叉点呢，去到任何一个序号都算完成任务
        line_1[po_from2] = 0;
        line_1[po_from] = 0;  // 由于是可以绕过自己的，所以....对于自己来说是无子的啦！
        if ((po_from == 0 && po_to == 23) || (po_from == 23 && po_to == 0) || (po_from == 17 && po_to == 18) || (po_from == 11 && po_to == 12) || (po_from == 12 && po_to == 11) || (po_from == 18 && po_to == 17) || (po_from == 5 && po_to == 6) || (po_from == 6 && po_to == 5))
            return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;  // 特判，直接在旋吃轨道两旁的当然可以吃。
        int IS_xuanchi = 0;
        for (int i = po_from + 1;; i++) {  // 向着序号增大的方向吃
            if (i == 24)                   // 这是一个首位相连的链。
                i = 0;
            if ((i == po_to || i == po_to2) && IS_xuanchi == 1)  // 只要到达任意一个序号都可以+通过旋吃弧线
                return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
            if (i == 6 || i == 5 || i == 11 || i == 12 || i == 17 || i == 18 || i == 0 || i == 23)  // 只要到达这几个地方，就可以认为这是通过旋吃弧线的。
                IS_xuanchi = 1;
            if (line_1[i] != 0)  // 碰到阻碍的话就无法前进了。
                break;
        }
        IS_xuanchi = 0;                    // 旋吃归零
        for (int i = po_from - 1;; i--) {  // 向着序号减小的方向吃
            if (i == -1)
                i = 23;
            if ((i == po_to || i == po_to2) && IS_xuanchi == 1)
                return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
            if (i == 6 || i == 5 || i == 11 || i == 12 || i == 17 || i == 18 || i == 0 || i == 23)
                IS_xuanchi = 1;
            if (line_1[i] != 0)
                break;
        }
        if (po_from == 1 || po_from == 4 || po_from == 10 || po_from == 16) {  // 同上
            IS_xuanchi = 0;
            for (int i = po_from2 + 1;; i++) {
                if (i == 24)
                    i = 0;
                if ((i == po_to || i == po_to2) && IS_xuanchi == 1)
                    return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                if (i == 6 || i == 5 || i == 11 || i == 12 || i == 17 || i == 18 || i == 0 || i == 23)
                    IS_xuanchi = 1;
                if (line_1[i] != 0)
                    break;
            }
            IS_xuanchi = 0;
            for (int i = po_from2 - 1;; i--) {
                if (i == -1)
                    i = 23;

                if ((i == po_to || i == po_to2) && IS_xuanchi == 1)
                    return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                if (i == 6 || i == 5 || i == 11 || i == 12 || i == 17 || i == 18 || i == 0 || i == 23)
                    IS_xuanchi = 1;
                if (line_1[i] != 0)
                    break;
            }
        }
        if (line_from == 1 || line_to == 1)
            return SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;
    }
    if (line_from == 2 || line_to == 2 || (line_to == 3 && line_from == 3)) {  // 另外一条路径上，与前面基本相同，所以没有特意标注释
        for (unsigned int i = 0; i <= 5; i++) {
            if ((*board_)[2][i]->GetColor() == PieceColor::BLACK || (*board_)[2][i]->GetColor() == PieceColor::WHITE) {
                line_2[i] = 1;
            }
            if ((*board_)[i][3]->GetColor() == PieceColor::BLACK || (*board_)[i][3]->GetColor() == PieceColor::WHITE) {
                line_2[i + 6] = 1;
            }
            if ((*board_)[3][5 - i]->GetColor() == PieceColor::BLACK || (*board_)[3][5 - i]->GetColor() == PieceColor::WHITE) {
                line_2[i + 12] = 1;
            }
            if ((*board_)[5 - i][2]->GetColor() == PieceColor::BLACK || (*board_)[5 - i][2]->GetColor() == PieceColor::WHITE) {
                line_2[i + 18] = 1;
            }
        }
        po_from = weizhi_line_2(move.from.x, move.from.y);
        po_to = weizhi_line_2(move.to.x, move.to.y);  //???z
        int po_to2 = 100, po_from2 = 100;
        if (po_from == 3 || po_from == 9 || po_from == 15)
            po_from2 = po_from + 5;
        if (po_from == 2)
            po_from2 = 21;
        line_2[po_from2] = 0;
        line_2[po_from] = 0;
        if (po_to == 3 || po_to == 9 || po_to == 15 || po_to == 2)  // 3=8 9=14 15=20 21=2
        {
            if (po_to == 3 || po_to == 9 || po_to == 15) {
                po_to2 = po_to + 5;
            }
            if (po_to == 2)
                po_to2 = 21;
        }
        if ((po_from == 0 && po_to == 23) || (po_from == 23 && po_to == 0) || (po_from == 17 && po_to == 18) || (po_from == 11 && po_to == 12) || (po_from == 12 && po_to == 11) || (po_from == 18 && po_to == 17) || (po_from == 5 && po_to == 6) || (po_from == 6 && po_to == 5))
            return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
        int IS_xuanchi = 0;
        for (int i = po_from + 1;; i++) {
            if (i == 24)
                i = 0;

            if ((i == po_to || i == po_to2) && IS_xuanchi == 1)
                return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
            if (i == 6 || i == 5 || i == 11 || i == 12 || i == 17 || i == 18 || i == 0 || i == 23)
                IS_xuanchi = 1;
            if (line_2[i] != 0)
                break;
        }
        IS_xuanchi = 0;
        for (int i = po_from - 1;; i--) {
            if (i == -1)
                i = 23;

            if ((i == po_to || i == po_to2) && IS_xuanchi == 1)
                return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
            if (i == 6 || i == 5 || i == 11 || i == 12 || i == 17 || i == 18 || i == 0 || i == 23)
                IS_xuanchi = 1;
            if (line_2[i] != 0)
                break;
        }
        if (po_from == 3 || po_from == 9 || po_from == 15 || po_from == 2)  // 3=8 9=14 15=20 21=2
        {
            if (po_from == 3 || po_from == 9 || po_from == 15) {
                po_from = po_from + 5;
            }
            if (po_from == 2)
                po_from = 21;
            IS_xuanchi = 0;
            for (int i = po_from + 1;; i++) {
                if (i == 24)
                    i = 0;

                if ((i == po_to || i == po_to2) && IS_xuanchi == 1)
                    return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                if (line_2[i] != 0)
                    break;
                if (i == 6 || i == 5 || i == 11 || i == 12 || i == 17 || i == 18 || i == 0 || i == 23)
                    IS_xuanchi = 1;
            }
            IS_xuanchi = 0;
            for (int i = po_from - 1;; i--) {
                if (i == -1)
                    i = 23;

                if ((i == po_to || i == po_to2) && IS_xuanchi == 1)
                    return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                if (line_2[i] != 0)
                    break;
                if (i == 6 || i == 5 || i == 11 || i == 12 || i == 17 || i == 18 || i == 0 || i == 23)
                    IS_xuanchi = 1;
            }
        }

        return SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;
    }
    return SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;
}
std::pair<SurakartaEndReason, SurakartaPlayer> SurakartaRuleManager::JudgeEnd(const SurakartaIllegalMoveReason reason) {
    SurakartaPlayer current_player = game_info_->current_player_;
    if (reason == SurakartaIllegalMoveReason::ILLIGAL || reason == SurakartaIllegalMoveReason::NOT_PLAYER_TURN || reason == SurakartaIllegalMoveReason::OUT_OF_BOARD || reason == SurakartaIllegalMoveReason::NOT_PIECE || reason == SurakartaIllegalMoveReason::NOT_PLAYER_PIECE || reason == SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE || reason == SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE) {
        if (current_player == SurakartaPlayer::BLACK)
            return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, SurakartaPlayer::WHITE);
        return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, SurakartaPlayer::BLACK);
    }
    int blacknum = 0, whitenum = 0;
    for (unsigned int i = 0; i < board_size_; i++) {
        for (unsigned int j = 0; j < board_size_; j++) {
            if ((*board_)[i][j]->GetColor() == PieceColor::BLACK) {
                blacknum++;
            }
            if ((*board_)[i][j]->GetColor() == PieceColor::WHITE) {
                whitenum++;
            }
        }
    }
    if (blacknum == 1 && current_player == SurakartaPlayer::WHITE && reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {
        return std::make_pair(SurakartaEndReason::CHECKMATE, SurakartaPlayer::WHITE);
    }
    if (whitenum == 1 && current_player == SurakartaPlayer::BLACK && reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {
        return std::make_pair(SurakartaEndReason::CHECKMATE, SurakartaPlayer::BLACK);
    }
    if (reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {
        return std::make_pair(SurakartaEndReason::NONE, SurakartaPlayer::NONE);
    }
    unsigned int NUMROUND = game_info_->num_round_;
    unsigned int last = game_info_->last_captured_round_;
    unsigned int no = game_info_->max_no_capture_round_;
    if (NUMROUND - last == no) {
        if (whitenum > blacknum)
            return std::make_pair(SurakartaEndReason::STALEMATE, SurakartaPlayer::WHITE);
        if (blacknum > whitenum)
            return std::make_pair(SurakartaEndReason::STALEMATE, SurakartaPlayer::BLACK);
        else
            return std::make_pair(SurakartaEndReason::STALEMATE, SurakartaPlayer::NONE);
    }
    if (current_player == SurakartaPlayer::BLACK)
        return std::make_pair(SurakartaEndReason::NONE, SurakartaPlayer::NONE);
    return std::make_pair(SurakartaEndReason::NONE, SurakartaPlayer::NONE);
}
std::unique_ptr<std::vector<SurakartaPosition>> SurakartaRuleManager::GetAllLegalTarget(const SurakartaPosition postion) {
    SurakartaPosition po;
    po = postion;
    return std::make_unique<std::vector<SurakartaPosition>>();  // noting
}
void SurakartaRuleManager::HelloWorld() {
    std::cout << "Hello World!" << std::endl;
}