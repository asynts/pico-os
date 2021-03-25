#pragma once

#include <Std/StringBuilder.hpp>
#include <Std/StringView.hpp>

namespace Std {
    template<typename Callback>
    void iterate_path_components(StringView path, Callback&& callback)
    {
        VERIFY(path.size() >= 1);
        VERIFY(path[0] == '/');
        path = path.substr(1);

        if (path.size() == 0)
            return;

        auto end_index = path.index_of('/');

        if (!end_index.is_valid()) {
            callback(path, true);
            return;
        } else {
            if (callback(path.trim(end_index.value()), false) == IterationDecision::Break)
                return;
            return iterate_path_components(path.substr(end_index.value()), move(callback));
        }
    }

    inline StringBuilder compute_absolute_path(StringView path)
    {
        StringBuilder builder;

        if (path.starts_with('/')) {
            builder.append(path);
            return builder;
        }

        // FIXME: Introduce process seperation
        StringView current_working_directory = "/";

        builder.append(current_working_directory);
        builder.append(path);

        return builder;
    }
}
