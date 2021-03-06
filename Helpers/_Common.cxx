#include "_Common.hxx"

#include <cassert>
#include <cstdint>

#ifndef HAVE_USABLE_WCHAR_T
static_assert(sizeof(wchar_t) == sizeof(PY_UNICODE_TYPE), "Please report this bug!");
#endif

namespace pbpp {

// 标准 PyBridget++ PyObject 对象布局
struct StdLayout {
    PyObject_HEAD
    void *cxx_obj;
    pbpp::FpOffsetBase fp_offset_base;
    pbpp::flag_t flags;
};

PyObject *PackAsTuple(const std::initializer_list<int> &numbers) {
    PyObject *ret = PyTuple_New(numbers.size());
    if (!ret) {
        return ret;
    }

    int i = 0;
    for (int n : numbers) {
        PyTuple_SET_ITEM(ret, i++, PyInt_FromLong(n));
    }

    return ret;
}

ExceptionArrayDestructor::ExceptionArrayDestructor(PyObject **exceptions)
    : exceptions(exceptions) {}

ExceptionArrayDestructor::~ExceptionArrayDestructor() {
    PyObject **p = exceptions;
    while (*p) {
        Py_DECREF(*p);
        p++;
    }
}

void CachePythonException(PyObject **exception) {
    PyObject *exc_type, *traceback;

    PyErr_Fetch(&exc_type, exception, &traceback);
    Py_XDECREF(exc_type);
    Py_XDECREF(traceback);
}

void RestorePythonExceptions(PyObject **exceptions, int n) {
    PyObject *error_list = PyList_New(n);
    for (int i = 0; i < n; i++) {
        PyList_SET_ITEM(error_list, i, PyObject_Str(exceptions[i]));
    }

    PyErr_SetObject(PyExc_TypeError, error_list);
    Py_DECREF(error_list);
}

namespace Types {

bool IsInteger(PyObject *obj) {
#if PY_VERSION_HEX < 0x03000000
    if (PyInt_Check(obj)) {
        return true;
    }
#endif

    return PyLong_Check(obj);
}

bool IsNumber(PyObject *obj) {
    return IsInteger(obj) || PyFloat_Check(obj);
}

template <typename T>
T _cast(PyObject *obj, T result) {
    if (result == -1 && PyErr_Occurred()) {
        PyErr_Clear();

        return static_cast<T>(ToDouble(obj));
    }

    return result;
}

long ToLong(PyObject *obj) {
    return _cast(obj, PyLong_AsLong(obj));
}

unsigned long ToUnsignedLong(PyObject *obj) {
    return _cast(obj, PyLong_AsUnsignedLong(obj));
}

long long ToLongLong(PyObject *obj) {
    return _cast(obj, PyLong_AsLongLong(obj));
}

unsigned long long ToUnsignedLongLong(PyObject *obj) {
#if PY_MAJOR_VERSION == 2
    /* The PyLong_AsUnsignedLongLong doesn't check the type of obj,
     * only accept argument of PyLong_Type, so we check it instead.
     */
    if (PyInt_Check(obj)) {
        return PyLong_AsUnsignedLong(obj);
    }
#endif

    return _cast(obj, PyLong_AsUnsignedLongLong(obj));
}

#define ERROR_MSG(T) "Python int too large to convert to C/C++ " #T

int ToInt(PyObject *obj) {
    return _cast(obj, PyInt_AsSsize_t(obj));
}

unsigned int ToUnsignedInt(PyObject *obj) {
    unsigned long result = PyLong_AsUnsignedLong(obj);
    if (result == static_cast<unsigned int>(-1) && PyErr_Occurred()) {
        PyErr_Clear();

        return static_cast<unsigned int>(ToDouble(obj));
    }

    if (result > UINT_MAX) {
        PyErr_SetString(PyExc_OverflowError, ERROR_MSG(unsigned int));

        return -1;
    }

    return static_cast<unsigned int>(result);
}

double ToDouble(PyObject *obj) {
    double result = PyFloat_AsDouble(obj);
    if (result == -1 && PyErr_Occurred()) {
        PyErr_Clear();

        return PyLong_AsDouble(obj);
    }

    return result;
}

template <typename T, int MAX, int MIN>
T _cast_down(PyObject *obj, const char *error_msg) {
    int result = ToInt(obj);
    if (result == -1 && PyErr_Occurred()) {
        return -1;
    }

    if (result > MAX || result < MIN) {
        PyErr_SetString(PyExc_OverflowError, error_msg);

        return -1;
    }

    return result;
}

short ToShort(PyObject *obj) {
    return _cast_down<short, SHRT_MAX, SHRT_MIN>(obj, ERROR_MSG(short));
}

unsigned short ToUnsignedShort(PyObject *obj) {
    return _cast_down<unsigned short, USHRT_MAX, 0>
        (obj, ERROR_MSG(unsigned short));
}

wchar_t ToWChar(PyObject *obj) {
    return _cast_down<wchar_t, WCHAR_MAX, 0>(obj, ERROR_MSG(wchar_t));
}

char ToChar(PyObject *obj) {
    return _cast_down<char, CHAR_MAX, CHAR_MIN>(obj, ERROR_MSG(char));
}

unsigned char ToUnsignedChar(PyObject *obj) {
    return _cast_down<unsigned char, UCHAR_MAX, 0>
        (obj, ERROR_MSG(unsigned char));
}

} // namespace Types

void RegisterEnumValues(EnumValue *val, PyObject *tp_dict) {
    while (val->name) {
        PyObject *pylong = PyInt_FromLong(val->value);
        PyDict_SetItemString(tp_dict, val->name, pylong);
        Py_DECREF(pylong);

        val++;
    }
}

PyObjectPtr::~PyObjectPtr() {
    release();
}

void PyObjectPtr::release() {
    if (!m_borrowed && m_ptr) {
#ifdef PBPP_SUPPORT_THREAD
        assert(PyGILState_Check());
#endif
        Py_DECREF(m_ptr);
        m_ptr = nullptr;
    }
}

#ifdef PBPP_SUPPORT_THREAD

#if PY_VERSION_HEX < 0x03000000
int PyGILState_Check() {
    return PyThreadState_Get() == PyGILState_GetThisThreadState();
}
#endif

PyGILState_STATE BeginBlockThreads() {
    if (!Py_IsInitialized()) {
        return PyGILState_LOCKED;
    }

    return PyGILState_Ensure();
}

void EndBlockThreads(PyGILState_STATE blocked) {
    if (!Py_IsInitialized()) {
        return;
    }

    PyGILState_Release(blocked);
}

ThreadBlocker::ThreadBlocker()
    : m_state(BeginBlockThreads()), m_disabled(false) {}

ThreadBlocker::~ThreadBlocker() {
    if (!m_disabled) {
        EndBlockThreads(m_state);
    }
}

void ThreadBlocker::Disable() {
    assert(!m_disabled);

    EndBlockThreads(m_state);
    m_disabled = true;
}

#endif // PBPP_SUPPORT_THREAD

CxxException::CxxException(PyObject *exception, const char *desc)
    : exception(exception), desc(desc) {
    PBPP_NEW_THREAD_BLOCKER

    PyErr_SetString(exception, desc);
    PyErr_Print();
}

#define EXCEPTION_CTOR(cls, ex) \
cls::cls(const char *desc) \
    : CxxException(ex, desc) {}

EXCEPTION_CTOR(PureVirtualFunctionNotImplemented, PyExc_NotImplementedError)
EXCEPTION_CTOR(CallPyMethodError, PyExc_RuntimeError)
EXCEPTION_CTOR(NullReferenceError, PyExc_ReferenceError)
EXCEPTION_CTOR(ArgumentTypeError, PyExc_TypeError)

namespace Types {

ConstCharPtr::ConstCharPtr(PyObject *obj) {
#if PY_VERSION_HEX >= 0x03000000
    m_bytes = PyUnicode_AsMBCSString(obj);
#else
    m_obj = obj;
#endif
}

ConstCharPtr::~ConstCharPtr() {
#if PY_VERSION_HEX >= 0x03000000
    if (m_bytes) {
        Py_DECREF(m_bytes);
        m_bytes = nullptr;
    }
#endif
}

ConstCharPtr::operator const char *() const {
#if PY_VERSION_HEX >= 0x03000000
    return m_bytes ? PyBytes_AsString(m_bytes) : nullptr;
#else
    return PyString_AsString(m_obj);
#endif
}

ConstWcharPtr::ConstWcharPtr(PyObject *unicode) {
#if PY_VERSION_HEX >= 0x03000000
    m_buf = PyUnicode_AsWideCharString(unicode, nullptr);
#else
    m_obj = unicode;
#endif
}

ConstWcharPtr::~ConstWcharPtr() {
#if PY_VERSION_HEX >= 0x03000000
    if (m_buf) {
        PyMem_Free(m_buf);
        m_buf = nullptr;
    }
#endif
}

ConstWcharPtr::operator const wchar_t *() const {
#if PY_VERSION_HEX >= 0x03000000
    return m_buf;
#else
    return PyUnicode_AsUnicode(m_obj);
#endif
}

} // namespace Types

} // namespace pbpp

#if PY_VERSION_HEX >= 0x03000000

PyObject *PyString_FromString(const char *s) {
    return PyUnicode_DecodeMBCS(s, strlen(s), nullptr);
}

#endif
