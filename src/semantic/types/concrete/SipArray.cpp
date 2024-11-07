#include "SipArray.h"
#include "TipTypeVisitor.h"

SipArray::SipArray(std::vector<std::shared_ptr<TipType>> elements)
    : TipCons(elements) {}

std::ostream &SipArray::print(std::ostream &out) const
{
  out << "[";
  bool first = true;
  int i = 0;
  for (auto &elem : arguments)
  {
    if (first)
    {
      out << *elem;
      first = false;
      continue;
    }
    out << "," << *elem;
  }
  out << "]";
  return out;
}

// This does not obey the semantics of alpha init values
bool SipArray::operator==(const TipType &other) const
{
  auto sipArray = dynamic_cast<const SipArray *>(&other);
  if (!sipArray)
  {
    return false;
  }

  if (arity() != sipArray->arity())
  {
    return false;
  }

  for (int i = 0; i < arity(); i++)
  {
    if (*(arguments.at(i)) != *(sipArray->arguments.at(i)))
    {
      return false;
    }
  }

  return true;
}

bool SipArray::operator!=(const TipType &other) const
{
  return !(*this == other);
}

std::vector<std::shared_ptr<TipType>> &SipArray::getElements()
{
  return arguments;
}

void SipArray::accept(TipTypeVisitor *visitor)
{
  if (visitor->visit(this))
  {
    for (auto a : arguments)
    {
      a->accept(visitor);
    }
  }
  visitor->endVisit(this);
}
