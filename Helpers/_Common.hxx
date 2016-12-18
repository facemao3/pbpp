/***************************************************************
 * Name:      _Common.hxx
 * Purpose:   һЩ���ø�����ʩ
 * Author:    Wang Xiaoning (vanxining@139.com)
 * Created:   2014
 **************************************************************/
#pragma once
#include "_Python.hxx"

#ifndef PyVarObject_HEAD_INIT
#   define PyVarObject_HEAD_INIT(type, size) \
           PyObject_HEAD_INIT(type) size,
#endif

#if PY_VERSION_HEX >= 0x03000000
    typedef void *cmpfunc;
#   define PyString_FromString(a) PyBytes_FromString(a)
#   define Py_TPFLAGS_CHECKTYPES 0 // This flag doesn't exist in Python 3
#endif

#if __GNUC__ > 2
#   define PBPP_UNUSED(param) param __attribute__((__unused__))
#elif __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#   define PBPP_UNUSED(param) __attribute__((__unused__)) param
#else
#   define PBPP_UNUSED(param)
#endif

#if PY_VERSION_HEX >= 0x03000000
#   define EXPORT_MOD(mod_name) \
        PyMODINIT_FUNC PyInit_ ## mod_name() { \
            return mod_name(nullptr); \
        } \
    
#   define CreateModule(mod_name) PyModule_Create(&mod_name ## _moduledef)
#else
#   define EXPORT_MOD(mod_name) \
        PyMODINIT_FUNC init ## mod_name() { \
            mod_name(nullptr); \
        } \
    
#   define CreateModule(mod_name) \
        Py_InitModule3((char *) mod_name, __methods, nullptr)
#endif

#define ___OVERLOAD___()
#define ___SCOPE___()

//////////////////////////////////////////////////////////////////////////

/// ��������ָ��ƫ��
/// 
/// ��Ϊ Python �����ǵ� C++ �����������ԣ�һ����֪���������� Python ������ʹ��
/// һ����������ķ���/��Ա����ʱ��Python ʹ�õ���������� PyObject ��������
/// �� cxx_obj ������������ָ�롣
/// ��������£�������Ҫ�ֶ������������ָ��תΪ��������ָ�롣
typedef void *(*FpOffsetBase)(void *cxx_obj, PyTypeObject *base_type);

namespace pbpp {

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
}

}

typedef unsigned short pbpp_flag_t;

//////////////////////////////////////////////////////////////////////////

#include <initializer_list>

namespace pbpp {

/// ��ȡ��������Ĳ�������
int GetMethodArgsCount(PyObject *py_method);

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

    NullReferenceError(const char *desc);
};

/// �������ʹ���
class ArgumentTypeError : public CxxException {
public:

    /// ���캯��
    ArgumentTypeError(const char *desc);
};

} // namespace pbpp

//////////////////////////////////////////////////////////////////////////

/// PyObject ����ָ��
/// 
/// @todo ���� pbpp �����ռ�
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

namespace pbpp {

/// ���ڹ���ǿ����ö�ٵ������龰
struct ScopedEnumDummy {};

} // namespace pbpp

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

inline PyThreadState *PyBeginAllowThreads() {
    return PyEval_SaveThread(); // Py_BEGIN_ALLOW_THREADS
}

inline void PyEndAllowThreads(PyThreadState *saved) {
    PyEval_RestoreThread(saved); // Py_END_ALLOW_THREADS
}

// Calls from C++ back to Python code, or even any PyObject
// manipulations, PyDECREF's and etc. are wrapped in calls to 
// these functions:

PyGILState_STATE PyBeginBlockThreads();
void PyEndBlockThreads(PyGILState_STATE blocked);

/// һ�� RAII ��
class PyThreadBlocker {
public:

    /// ���캯��
    PyThreadBlocker();
    PyThreadBlocker(const PyThreadBlocker &) = delete;
    PyThreadBlocker &operator=(const PyThreadBlocker &) = delete;

    /// ��������
    ~PyThreadBlocker();

    /// ��ǰ���������߳�����
    void Disable();

private:

    PyGILState_STATE m_state;
    bool m_disabled; // �����Ƿ��ѱ����ã�(�����߳��ܷ����У�)
};

#define PBPP_BEGIN_ALLOW_THREADS PyThreadState *_saved = PyBeginAllowThreads();
#define PBPP_END_ALLOW_THREADS PyEndAllowThreads(_saved);

#define PBPP_NEW_THREAD_BLOCKER PyThreadBlocker _blocker;
#define PBPP_DISABLE_THREAD_BLOCKER _blocker.Disable();

#else

#define PBPP_BEGIN_ALLOW_THREADS
#define PBPP_END_ALLOW_THREADS

#define PBPP_NEW_THREAD_BLOCKER
#define PBPP_DISABLE_THREAD_BLOCKER

#endif
