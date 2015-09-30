/* Material.h
 *
 * Kubo Ryosuke
 */

#ifndef SUNFISH_MATERIAL__
#define SUNFISH_MATERIAL__

#include "core/def.h"
#include "core/base/Piece.h"
#include "Value.h"
#include "material_values.h"
#include <cstdint>

namespace sunfish {

namespace material {

#if defined(NLEARN)
CONSTEXPR_CONST int16_t Pawn       = MATERIAL_VALUE_PAWN;
CONSTEXPR_CONST int16_t Lance      = MATERIAL_VALUE_LANCE;
CONSTEXPR_CONST int16_t Knight     = MATERIAL_VALUE_KNIGHT;
CONSTEXPR_CONST int16_t Silver     = MATERIAL_VALUE_SILVER;
CONSTEXPR_CONST int16_t Gold       = MATERIAL_VALUE_GOLD;
CONSTEXPR_CONST int16_t Bishop     = MATERIAL_VALUE_BISHOP;
CONSTEXPR_CONST int16_t Rook       = MATERIAL_VALUE_ROOK;
CONSTEXPR_CONST int16_t Tokin      = MATERIAL_VALUE_TOKIN;
CONSTEXPR_CONST int16_t Pro_lance  = MATERIAL_VALUE_PRO_LANCE;
CONSTEXPR_CONST int16_t Pro_knight = MATERIAL_VALUE_PRO_KNIGHT;
CONSTEXPR_CONST int16_t Pro_silver = MATERIAL_VALUE_PRO_SILVER;
CONSTEXPR_CONST int16_t Horse      = MATERIAL_VALUE_HORSE;
CONSTEXPR_CONST int16_t Dragon     = MATERIAL_VALUE_DRAGON;

CONSTEXPR_CONST int16_t PawnEx       = Pawn * 2;
CONSTEXPR_CONST int16_t LanceEx      = Lance * 2;
CONSTEXPR_CONST int16_t KnightEx     = Knight * 2;
CONSTEXPR_CONST int16_t SilverEx     = Silver * 2;
CONSTEXPR_CONST int16_t GoldEx       = Gold * 2;
CONSTEXPR_CONST int16_t BishopEx     = Bishop * 2;
CONSTEXPR_CONST int16_t RookEx       = Rook * 2;
CONSTEXPR_CONST int16_t TokinEx      = Tokin + Pawn;
CONSTEXPR_CONST int16_t Pro_lanceEx  = Pro_lance + Lance;
CONSTEXPR_CONST int16_t Pro_knightEx = Pro_knight + Knight;
CONSTEXPR_CONST int16_t Pro_silverEx = Pro_silver + Silver;
CONSTEXPR_CONST int16_t HorseEx      = Horse + Bishop;
CONSTEXPR_CONST int16_t DragonEx     = Dragon + Rook;

/**
 * $B6p3d$r<hF@$7$^$9!#(B
 */
inline Value piece(const Piece& piece) {
  static const Value values[] = {
    /*  0 */ material::Pawn,
    /*  1 */ material::Lance,
    /*  2 */ material::Knight,
    /*  3 */ material::Silver,
    /*  4 */ material::Gold,
    /*  5 */ material::Bishop,
    /*  6 */ material::Rook,
    /*  7 */ Value::PieceInf,
    /*  8 */ material::Tokin,
    /*  9 */ material::Pro_lance,
    /* 10 */ material::Pro_knight,
    /* 11 */ material::Pro_silver,
    /* 12 */ 0,
    /* 13 */ material::Horse,
    /* 14 */ material::Dragon,
    /* 15 */ Value::PieceInf,
    /* 16 */ material::Pawn,
    /* 17 */ material::Lance,
    /* 18 */ material::Knight,
    /* 19 */ material::Silver,
    /* 20 */ material::Gold,
    /* 21 */ material::Bishop,
    /* 22 */ material::Rook,
    /* 23 */ Value::PieceInf,
    /* 24 */ material::Tokin,
    /* 25 */ material::Pro_lance,
    /* 26 */ material::Pro_knight,
    /* 27 */ material::Pro_silver,
    /* 28 */ 0,
    /* 29 */ material::Horse,
    /* 30 */ material::Dragon,
    /* 31 */ Value::PieceInf,
  };
  return values[piece.index()];
}

/**
 * $B6p$r<h$C$?;~$NJQ2=CM$r<hF@$7$^$9!#(B
 */
inline Value pieceExchange(const Piece& piece) {
  static const Value values[] = {
    /*  0 */ material::PawnEx,
    /*  1 */ material::LanceEx,
    /*  2 */ material::KnightEx,
    /*  3 */ material::SilverEx,
    /*  4 */ material::GoldEx,
    /*  5 */ material::BishopEx,
    /*  6 */ material::RookEx,
    /*  7 */ Value::PieceInfEx,
    /*  8 */ material::TokinEx,
    /*  9 */ material::Pro_lanceEx,
    /* 10 */ material::Pro_knightEx,
    /* 11 */ material::Pro_silverEx,
    /* 12 */ 0,
    /* 13 */ material::HorseEx,
    /* 14 */ material::DragonEx,
    /* 15 */ Value::PieceInfEx,
    /* 16 */ material::PawnEx,
    /* 17 */ material::LanceEx,
    /* 18 */ material::KnightEx,
    /* 19 */ material::SilverEx,
    /* 20 */ material::GoldEx,
    /* 21 */ material::BishopEx,
    /* 22 */ material::RookEx,
    /* 23 */ Value::PieceInfEx,
    /* 24 */ material::TokinEx,
    /* 25 */ material::Pro_lanceEx,
    /* 26 */ material::Pro_knightEx,
    /* 27 */ material::Pro_silverEx,
    /* 28 */ 0,
    /* 29 */ material::HorseEx,
    /* 30 */ material::DragonEx,
    /* 31 */ Value::PieceInfEx,
  };
  return values[piece.index()];
}

/**
 * $B6p$,@.$C$?;~$NJQ2=CM$r<hF@$7$^$9!#(B
 */
inline Value piecePromote(const Piece& piece) {
  static const Value values[] = {
    /*  0 */ material::Tokin - material::Pawn,
    /*  1 */ material::Pro_lance - material::Lance,
    /*  2 */ material::Pro_knight - material::Knight,
    /*  3 */ material::Pro_silver - material::Silver,
    /*  4 */ 0,
    /*  5 */ material::Horse - material::Bishop,
    /*  6 */ material::Dragon - material::Rook,
    /*  7 */ 0,
    /*  8 */ 0,
    /*  9 */ 0,
    /* 10 */ 0,
    /* 11 */ 0,
    /* 12 */ 0,
    /* 13 */ 0,
    /* 14 */ 0,
    /* 15 */ 0,
    /* 16 */ material::Tokin - material::Pawn,
    /* 17 */ material::Pro_lance - material::Lance,
    /* 18 */ material::Pro_knight - material::Knight,
    /* 19 */ material::Pro_silver - material::Silver,
    /* 20 */ 0,
    /* 21 */ material::Horse - material::Bishop,
    /* 22 */ material::Dragon - material::Rook,
    /* 23 */ 0,
    /* 24 */ 0,
    /* 25 */ 0,
    /* 26 */ 0,
    /* 27 */ 0,
    /* 28 */ 0,
    /* 29 */ 0,
    /* 30 */ 0,
    /* 31 */ 0,
  };
  return values[piece.index()];
}
#else
extern int16_t Pawn;
extern int16_t Lance;
extern int16_t Knight;
extern int16_t Silver;
extern int16_t Gold;
extern int16_t Bishop;
extern int16_t Rook;
extern int16_t Tokin;
extern int16_t Pro_lance;
extern int16_t Pro_knight;
extern int16_t Pro_silver;
extern int16_t Horse;
extern int16_t Dragon;

extern int16_t PawnEx;
extern int16_t LanceEx;
extern int16_t KnightEx;
extern int16_t SilverEx;
extern int16_t GoldEx;
extern int16_t BishopEx;
extern int16_t RookEx;
extern int16_t TokinEx;
extern int16_t Pro_lanceEx;
extern int16_t Pro_knightEx;
extern int16_t Pro_silverEx;
extern int16_t HorseEx;
extern int16_t DragonEx;

Value piece(const Piece& piece);
Value pieceExchange(const Piece& piece);
Value piecePromote(const Piece& piece);

void updateEx();
void writeFile();
#endif // defined(NLEARN)

} // namespace material

} // namespace sunfish

#endif // SUNFISH_MATERIAL__
