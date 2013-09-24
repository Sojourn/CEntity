#ifndef QUERYRESULT_H
#define QUERYRESULT_H

template<class IteratorType>
class QueryResult
{
public:
	typedef typename IteratorType iterator_type;

	QueryResult(std::pair<iterator_type, iterator_type> range) :
		_range(range)
	{}

	inline iterator_type begin() const
	{
		return _range.first;
	}
	
	inline iterator_type end() const
	{
		return _range.second;
	}

private:
	const std::pair<iterator_type, iterator_type> _range;
};

#endif // QUERYRESULT_H