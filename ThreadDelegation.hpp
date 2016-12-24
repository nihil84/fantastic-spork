namespace ThreadDelegation {

namespace _details {

class IDelegator : public QObject {
	Q_OBJECT
public:
	IDelegator() {
		connect(this, SIGNAL(invoke()), SLOT(onInvoke()));
	}
signals:
	void invoke();
private slots:
	virtual void onInvokation() = 0;
};

template <typename R, typename F>
class Delegator : public IDelegator {
public:
    typedef R ResultType;
    Delegator(F fun) : m_fun(fun) { }
    QFuture<ResultType> delegate(QThread& thread);
private:
    QFutureInterface<ReturnType> m_res;
    typename F m_fun;
    void onInvokation();
}

template <typename F>
class Delegator<void, F> : public IDelegator {
public:	
	typedef void ResultType;
	// [...] everything exactly as above
};

template <typename R, typename F>
QFuture<R> Delegator<R, F>::delegate(QThread& thread) {
	moveToThread(thread); //[1]
	QFuture<ResultType> future = m_res.future(); //[2]
	emit invoke(); //[3]
	return future;
}

template <typename R, typename F>
void Delegator<R, F>::onInvokation() {
	ResultType result = m_fun(); //[4]
	m_res.reportFinished(&result); //[5]
	this->deleteLater();//[6]
}

template <typename F>
void Delegator<void, F>::onInvokation() {
	m_fun();
	m_res.reportFinished();
	this->deleteLater();
}

} // end of namespace _details


template <typename R>
struct Result {
	template <typename F>
	static QFuture<R> delegate(QThread& thread, F fun) {
		return (new _details::Delegator<R,F>(fun))->delegate(thread);
	}
}

} //end of namespace ThreadDelegation