/* LearningTemplates.h
 * 
 * Kubo Ryosuke
 */

#ifndef SUNFISH_LEARNINGTEMPLATES__
#define SUNFISH_LEARNINGTEMPLATES__

#ifndef NLEARN

#include "./FV.h"

namespace sunfish {

class LearningTemplates {
private:

  LearningTemplates();

public:

  template <class Type, class Func>
  static void symmetrize(Feature<Type>& fv, Func&& f) {
    // king-king-piece
    SQUARE_EACH(bking0) {
      Square bking1 = bking0.sym();
      if (bking0.index() > bking1.index()) {
        continue;
      }

      SQUARE_EACH(wking0) {
        if (bking0.index() == wking0.index()) {
          continue;
        }
        Square wking1 = wking0.sym();
        if (bking0.index() == bking1.index() && wking0.index() > wking1.index()) {
          continue;
        }

        for (int index0 = 0; index0 < KKP_MAX; index0++) {
          int index1 = symmetrizeKkpIndex(index0);
          if (bking0.index() == bking1.index() && wking0.index() > wking1.index() && index0 >= index1) {
            continue;
          }
          f(fv.t_->kkp[bking0.index()][wking0.index()][index0], fv.t_->kkp[bking1.index()][wking1.index()][index1]);
        }
      }
    }

    // king-gold-piece
    SQUARE_EACH(king0) {
      Square king1 = king0.sym();
      if (king0.index() > king1.index()) {
        continue;
      }

      for (int x0 = 0; x0 < KG_MAX; x0++) {
        int x1 = symmetrizeKgIndex(x0);
        if (king0.index() == king1.index() && x0 > x1) {
          continue;
        }

        for (int y0 = 0; y0 < KGP_MAX; y0++) {
          int y1 = symmetrizeKgpIndex(y0);
          if (king0.index() == king1.index() && x0 == x1 && y0 > y1) {
            continue;
          }

          f(fv.t_->kgp[king0.index()][x0][y0], fv.t_->kgp[king1.index()][x1][y1]);
        }
      }
    }

    // king-gold-gold
    SQUARE_EACH(king0) {
      Square king1 = king0.sym();
      if (king0.index() > king1.index()) {
        continue;
      }

      for (int x0 = 0; x0 < KG_MAX; x0++) {
        int x1 = symmetrizeKgIndex(x0);
        if (king0.index() == king1.index() && x0 > x1) {
          continue;
        }

        for (int y0 = 0; y0 <= x0; y0++) {
          int y1 = symmetrizeKgIndex(y0);
          if (king0.index() == king1.index() && x0 == x1 && y0 > y1) {
            continue;
          }

          int index0 = kgg_index(x0, y0);
          int index1 = kgg_index_safe(x1, y1);
          f(fv.t_->kgg[king0.index()][index0], fv.t_->kgg[king1.index()][index1]);
        }
      }
    }
  }

  template <class Type, class Func>
  static void symmetrize(FeatureX<Type>& fv, Func&& f) {
    // king-piece
    SQUARE_EACH(king0) {
      Square king1 = king0.sym();
      if (king0.index() > king1.index()) {
        continue;
      }

      for (int index0 = 0; index0 < KKP_MAX; index0++) {
        int index1 = symmetrizeKkpIndex(index0);
        if (king0.index() == king1.index() && index0 >= index1) {
          continue;
        }
        f(fv.t_->kpb[king0.index()][index0], fv.t_->kpb[king1.index()][index1]);
        f(fv.t_->kpw[king0.index()][index0], fv.t_->kpw[king1.index()][index1]);
      }
    }
  }

};

} // namespace sunfish

#endif // NLEARN

#endif // SUNFISH_LEARNINGTEMPLATES__
