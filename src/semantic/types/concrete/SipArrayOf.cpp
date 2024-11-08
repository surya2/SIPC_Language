#include "SipArrayOf.h"
#include "TipTypeVisitor.h"

SipArrayOf::SipArrayOf(std::shared_ptr<TipType> length, std::shared_ptr<TipType> element)
    : length(length), type(element) {}

std::ostream &SipArrayOf::print(std::ostream &out) const
{
  out << *type;
  out << "[";
  out << *length << " of " << *type;
  out << "]";
  return out;
}

// This does not obey the semantics of alpha init values
bool SipArrayOf::operator==(const TipType &other) const
{
  auto sipArrayOf = dynamic_cast<const SipArrayOf *>(&other);
  if (!sipArrayOf)
  {
    return false;
  }

  if (*length != *sipArrayOf->length || *type != *sipArrayOf->type)
  {
    return false;
  }

  return true;
}

bool SipArrayOf::operator!=(const TipType &other) const
{
  return !(*this == other);
}

TipType *SipArrayOf::getLength()
{
  return length.get();
}

TipType *SipArrayOf::getElement()
{
  return type.get();
}

void SipArrayOf::accept(TipTypeVisitor *visitor)
{
  if (visitor->visit(this))
  {
    getLength()->accept(visitor);
    getElement()->accept(visitor);
  }
  visitor->endVisit(this);
}
