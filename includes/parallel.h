
#ifndef __PARALLEL_H__
#define __PARALLEL_H__

#include "imagec.h"
#include <omp.h>

#define PARALLEL(__image_input, _slice_count, __operation) ({\
    image_t _image = (__image_input);\
    image_t _result = clone(_image);\
    int _height = _image.h;\
    int _width = _image.w;\
    int _vertical = (_height >= _width);\
    int _max_dim = _vertical ? _height : _width;\
    if (_slice_count > _max_dim) {\
        fprintf(stderr, "Error: invalid slice size.\n");\
    } else {\
        image_t _slices[_slice_count];\
        image_t _outputs[_slice_count];\
        int _base = _max_dim / _slice_count;\
        int _remainder = _max_dim % _slice_count;\
        \
        _Pragma("omp parallel for")\
        for (int i = 0; i < _slice_count; ++i) {\
            _slices[i] = _image;\
            int start = i * _base + (i < _remainder ? i : _remainder);\
            int size = _base + (i < _remainder ? 1 : 0);\
            \
            if (_vertical) {\
                _slices[i].bounds = boundaries(0, start, _width, start + size);\
            } else {\
                _slices[i].bounds = boundaries(start, 0, start + size, _height);\
            }\
            image_t * SLICE = &_slices[i];\
            __operation;\
            _outputs[i] = _slices[i];\
        }\
        \
        for (int i = 0; i < _slice_count; ++i) {\
            int start = i * _base + (i < _remainder ? i : _remainder);\
            if (_vertical) {\
                paste(_outputs[i], _result, 0, start);\
            } else {\
                paste(_outputs[i], _result, start, 0);\
            }\
        }\
    }\
    _result;\
})

#endif /* __PARALLEL_H__ */