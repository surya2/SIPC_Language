#include "SipArray.h"
#include "TipTypeVisitor.h"

SipArray::SipArray(std::vector<std::shared_ptr<TipType>> elements)
    : type(elements.front()), TipCons(std::vector<std::shared_ptr<TipType>>{elements.front()}) {}
SipArray::SipArray(std::shared_ptr<TipType> elementsType)
    : type(elementsType), TipCons(std::vector<std::shared_ptr<TipType>>{elementsType}) {}

std::ostream &SipArray::print(std::ostream &out) const
{
  out << *arguments.front();
  out << "[";
  out << *arguments.front();
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

  if (*arguments.front() == *sipArray->arguments.front())
  {
    return true;
  }
  else
  {
    return false;
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
