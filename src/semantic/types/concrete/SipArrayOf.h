#pragma once

#include "TipCons.h"
#include "TipType.h"
#include <ostream>
#include <string>
#include <vector>

/*!
 * \class SipArrayOf
 *
 * \brief A proper type representing an array
 */
class SipArrayOf : public TipCons
{
public:
  SipArrayOf() = delete;
  SipArrayOf(std::shared_ptr<TipType> length, std::shared_ptr<TipType> element);

  TipType *getLength();
  TipType *getElement();
  bool operator==(const TipType &other) const override;
  bool operator!=(const TipType &other) const override;

  void accept(TipTypeVisitor *visitor) override;

protected:
  std::ostream &print(std::ostream &out) const override;

private:
  std::shared_ptr<TipType> const length;
  std::shared_ptr<TipType> const type;
};
