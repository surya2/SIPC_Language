#pragma once

#include "TipCons.h"
#include "TipType.h"
#include <ostream>
#include <string>
#include <vector>

/*!
 * \class SipArray
 *
 * \brief A proper type representing an array
 */
class SipArray : public TipCons
{
public:
  SipArray() = delete;
  SipArray(std::vector<std::shared_ptr<TipType>> elements);

  std::vector<std::shared_ptr<TipType>> &getElements();
  bool operator==(const TipType &other) const override;
  bool operator!=(const TipType &other) const override;

  void accept(TipTypeVisitor *visitor) override;

protected:
  std::ostream &print(std::ostream &out) const override;

private:
  std::vector<std::string> const names;
};
