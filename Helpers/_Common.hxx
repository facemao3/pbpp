/***************************************************************
 * Name:      _Common.hxx
 * Purpose:   һЩ���ø�����ʩ
 * Author:    Wang Xiaoning (vanxining@139.com)
 * Created:   2014
 **************************************************************/
#pragma once
#include "_Python.hxx"

#include <initializer_list>

#if __GNUC__ > 2
#   define PBPP_UNUSED(param) param __attribute__((__unused__))
#elif __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#   define PBPP_UNUSED(param) __attribute__((__unused__)) param
#else
#   define PBPP_UNUSED(param)
#endif

//////////////////////////////////////////////////////////////////////////

namespace pbpp {

/// ��������ָ��ƫ��
///
/// ��Ϊ Python �����ǵ� C++ �����������ԣ�һ����֪���������� Python ������ʹ��
/// һ����������ķ���/��Ա����ʱ��Python ʹ�õ���������� PyObject ��������
/// �� cxx_obj ������������ָ�롣
/// ��������£�������Ҫ�ֶ������������ָ��תΪ��������ָ�롣
typedef void *(*FpOffsetBase)(void *cxx_obj, PyTypeObject *base_type);

/// Python �����Ӧ�� C++ ������������ڹ������
namespace LifeTime {
enum {
    /// C++ ������������ڸ��� Python ����Ĭ�ϣ�
    PYTHON = 1 << 0,

    /// C++ ������������ڶ����ڶ�Ӧ�� Python �����ҳ��� Python �����
    /// һ������
    ///
    /// һ��Ӧ�ó����� C++ ����Ҫ���� Python �������е��麯������ʱһ����Ȼ��
    /// Ҫ�������Ǵ�ʱ���뱣֤ Python ������Ȼ���ڡ�
    /// ע�⣺��������� C++ ����������� Python ������������ʱ Python ����
    /// ��Ȼ��Ч������ Python �˼���ʹ�øö�����ô�������Ϊ��δ����ġ�
    CXX = 1 << 1,

    /// ���õ� C++ �����������������Ӧ�� Python �����޹�
    ///
    /// ʵ���� C++ ������ Python �������ߵ�����ʱ��֮�����κι�ϵ��
    BORROWED = 1 << 2,
};
} // namespace LifeTime

typedef unsigned char flag_t;

//////////////////////////////////////////////////////////////////////////

/// ��һ�����ִ���� Python Ԫ��
PyObject *PackAsTuple(const std::initializer_list<int> &numbers);

/// �Զ�����һ���쳣��������
///
/// Ϊ֧�ֺ������أ������ڽ�����������ʱ��ʱ�����˳���ĳ������ʧ��ʱ
/// PyArg_ParseTupleAndKeywords() �����Ĳ������Ͳ�ƥ���쳣����
/// ������Ŀ�ľ��ǴӰ�����������ʱ�Զ�������Щ�������
class ExceptionArrayDestructor {
public:

    /// ���캯��
    ///
    /// @param exceptions ��Ҫ��֤�������һ��Ԫ��Ϊ nullptr
    ExceptionArrayDestructor(PyObject **exceptions);

    /// ��������
    ~ExceptionArrayDestructor();

private:

    PyObject **exceptions;
};

/// ȡ����ǰ Python �������쳣���󲢽��仺�浽 @a exception
void CachePythonException(PyObject **exception);

/// ��������쳣��������ת���� Python
///
/// ����û�к��ʵ����ص��á�
void RestorePythonExceptions(PyObject **exceptions, int n);

//////////////////////////////////////////////////////////////////////////

namespace Types {

/// �Ƿ�Ϊ����
///
/// ���� PyInt (Python 2.x) �Լ� PyLong��
bool IsInteger(PyObject *obj);

/// �Ƿ�Ϊ����
///
/// ���� PyInt (Python 2.x), PyLong, PyFloat��
bool IsNumber(PyObject *obj);

/// ת��Ϊ����
///
/// ������ @a obj �Ƿ�ȷʵΪһ�����֡�
/// @return ������ -1����Ҫ�����߼���Ƿ�����˴�������ȷ���ȷʵ�� -1��
int ToInt(PyObject *obj);

/// ת��Ϊ�޷�������
unsigned int ToUnsignedInt(PyObject *obj);

/// ת��Ϊ������
long ToLong(PyObject *obj);

/// ת��Ϊ�޷��ų�����
unsigned long ToUnsignedLong(PyObject *obj);

/// ת��Ϊ��������
long long ToLongLong(PyObject *obj);

/// ת��Ϊ�޷��ų�������
unsigned long long ToUnsignedLongLong(PyObject *obj);

/// ת��Ϊ˫���ȸ�����
double ToDouble(PyObject *obj);

/// ת��Ϊ������
short ToShort(PyObject *obj);

/// ת��Ϊ�޷��Ŷ�����
unsigned short ToUnsignedShort(PyObject *obj);

/// ת��Ϊ wchar_t
///
/// û�� unsigned wchar_t��
wchar_t ToWChar(PyObject *obj);

/// ת��Ϊ char
///
char ToChar(PyObject *obj);

/// ת��Ϊ unsigned char
unsigned char ToUnsignedChar(PyObject *obj);

/// ��������תΪָ��
template <typename T>
inline T *ToPointer(const T &ref) {
    return &const_cast<T &>(ref);
}

} // namespace Types

//////////////////////////////////////////////////////////////////////////

/// ���� C++ ���쳣
class CxxException {
public:

    /// ���캯��
    CxxException(PyObject *exception, const char *desc);

public:

    /// �쳣����
    PyObject *exception;
    const char *desc; // ���������ı�
};

/// ���麯��δʵ��
class PureVirtualFunctionNotImplemented : public CxxException {
public:

    /// ���캯��
    PureVirtualFunctionNotImplemented(const char *desc);
};

/// �� C++ ���� Python �������ִ���
///
/// ���ܵ�ԭ���з���ֵ��������͡�������ƥ�䡣
class CallPyMethodError : public CxxException {
public:

    /// ���캯��
    CallPyMethodError(const char *desc);
};

/// ������
class NullReferenceError : public CxxException {
public:

    /// ���캯��
    NullReferenceError(const char *desc);
};

/// �������ʹ���
class ArgumentTypeError : public CxxException {
public:

    /// ���캯��
    ArgumentTypeError(const char *desc);
};

//////////////////////////////////////////////////////////////////////////

/// PyObject ����ָ��
class PyObjectPtr {
public:

    /// ��ʽ���캯��
    explicit PyObjectPtr(PyObject *ptr, bool borrowed = false)
        : m_ptr(ptr), m_borrowed(borrowed) {}

    /// ��������
    ~PyObjectPtr();

    /// ��ȡ PyObject ָ��
    operator PyObject *() { return m_ptr; }
    PyObject *get() const { return m_ptr; }

    /// �����Ƿ�Ϊ��
    operator bool() { return m_ptr != nullptr;  }

    /// �ͷ� PyObject ָ��
    void release();

private:

    // ���ܸ���
    PyObjectPtr(const PyObjectPtr &) = delete;
    PyObjectPtr &operator=(const PyObjectPtr &) = delete;

private:

    PyObject *m_ptr;
    bool m_borrowed;
};

//////////////////////////////////////////////////////////////////////////

/// ���ڹ���ǿ����ö�ٵ������龰
struct ScopedEnumDummy {};

/// ö�� key-value ��
struct EnumValue {
    const char *name; /*! ���� */
    int value; /*! ö��ֵ */
};

/// ע��һ��ö�ټ�ֵ������
void RegisterEnumValues(EnumValue *val, PyObject *tp_dict);

//////////////////////////////////////////////////////////////////////////

#ifdef PBPP_SUPPORT_THREAD

/* Function to check whether the current thread has acquired the GIL.
 * Return 1 if the current thread is holding the GIL and 0 otherwise.
 *
 * This is a kind-of untested backport of
 * https://docs.python.org/3/c-api/init.html?highlight=pygilstate_check#c.PyGILState_Check
 * It can be useful in debugging deadlocks/crashes in python-c api
 * usage by checking whether the GIL is acquired or not where expected.
 *
 * From: https://github.com/pankajp/pygilstate_check
 */
#if PY_VERSION_HEX < 0x03000000
int PyGILState_Check();
#endif

// Calls from Python to C++ code are wrapped in calls to these
// functions:

inline PyThreadState *BeginAllowThreads() {
    return PyEval_SaveThread(); // Py_BEGIN_ALLOW_THREADS
}

inline void EndAllowThreads(PyThreadState *saved) {
    PyEval_RestoreThread(saved); // Py_END_ALLOW_THREADS
}

// Calls from C++ back to Python code, or even any PyObject
// manipulations, PyDECREF's and etc. are wrapped in calls to
// these functions:

PyGILState_STATE BeginBlockThreads();
void EndBlockThreads(PyGILState_STATE blocked);

/// һ�� RAII ��
class ThreadBlocker {
public:

    /// ���캯��
    ThreadBlocker();
    ThreadBlocker(const ThreadBlocker &) = delete;
    ThreadBlocker &operator=(const ThreadBlocker &) = delete;

    /// ��������
    ~ThreadBlocker();

    /// ��ǰ���������߳�����
    void Disable();

private:

    PyGILState_STATE m_state;
    bool m_disabled; // �����Ƿ��ѱ����ã�(�����߳��ܷ����У�)
};

#endif // #ifdef PBPP_SUPPORT_THREAD

} // namespace pbpp

#ifdef PBPP_SUPPORT_THREAD

#define PBPP_BEGIN_ALLOW_THREADS PyThreadState *py_thread_state = pbpp::BeginAllowThreads();
#define PBPP_END_ALLOW_THREADS pbpp::EndAllowThreads(py_thread_state);

#define PBPP_NEW_THREAD_BLOCKER pbpp::ThreadBlocker py_thread_blocker;
#define PBPP_DISABLE_THREAD_BLOCKER py_thread_blocker.Disable();

#else

#define PBPP_BEGIN_ALLOW_THREADS
#define PBPP_END_ALLOW_THREADS

#define PBPP_NEW_THREAD_BLOCKER
#define PBPP_DISABLE_THREAD_BLOCKER

#endif

#if PY_VERSION_HEX >= 0x03000000

#define PyInt_AsSsize_t PyLong_AsSsize_t
#define PyInt_FromSsize_t PyLong_FromSsize_t
#define PyInt_FromSize_t PyLong_FromSize_t
#define PyInt_FromLong PyLong_FromLong

#define PyString_Check PyUnicode_Check
PyObject *PyString_FromString(const char *s);

#endif // PY_VERSION_HEX >= 0x03000000

namespace pbpp {

namespace Types {

/// �����ַ���ָ�������
class ConstCharPtr {
public:

    /// ���캯��
    ConstCharPtr(PyObject *obj);

    /// ��������
    ~ConstCharPtr();

    /// ����ת������
    operator const char *() const;

private:

#if PY_VERSION_HEX >= 0x03000000
    PyObject *m_bytes;
#else
    PyObject *m_obj;
#endif
};

/// �������ַ���ָ�������
class ConstWcharPtr {
public:

    /// ���캯��
    ConstWcharPtr(PyObject *unicode);

    /// ��������
    ~ConstWcharPtr();

    /// ����ת������
    operator const wchar_t *() const;

private:

#if PY_VERSION_HEX >= 0x03000000
    wchar_t *m_buf;
#else
    PyObject *m_obj;
#endif
};

} // namespace Types

} // namespace pbpp
