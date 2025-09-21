/*
Copyright 2019 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include "svgnative/SVGRenderer.h"
#include "svgnative/Rect.h"

namespace SVGNative
{

Rect SVGRenderer::GetBounds(const Path&, const GraphicStyle&, const FillStyle&, const StrokeStyle&)
{
    throw "Bound calculation functionality not implemented in this port";
    return Rect{0, 0, 0, 0};
}

}