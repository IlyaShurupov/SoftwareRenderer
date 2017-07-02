#pragma once

/** @file */

#include "TriangleEquations.h"
#include "PixelData.h"

namespace swr {

/// Pixel shader base class.
/** Derive your own pixel shaders from this class and redefine the static
  variables to match your pixel shader requirements. */
template <class Derived>
class PixelShaderBase {
public:
	/// Tells the rasterizer to interpolate the z component.
	static const int InterpolateZ = false;

	/// Tells the rasterizer to interpolate the w component.
	static const int InterpolateW = false;

	/// Tells the rasterizer how many affine vars to interpolate.
	static const int AVarCount = 0;

	/// Tells the rasterizer how many perspective vars to interpolate.
	static const int PVarCount = 0;

	template <bool TestEdges>
	static void drawBlock(const TriangleEquations &eqn, int x, int y)
	{
		float xf = x + 0.5f;
		float yf = y + 0.5f;

		PixelData po;
		po.init(eqn, xf, yf, Derived::AVarCount, Derived::PVarCount, Derived::InterpolateZ, Derived::InterpolateW);

		EdgeData eo;
		if (TestEdges)
			eo.init(eqn, xf, yf);

		for (int yy = y; yy < y + BlockSize; yy++)
		{
			PixelData pi = copyPixelData(po);

			EdgeData ei;
			if (TestEdges)
				ei = eo;

			for (int xx = x; xx < x + BlockSize; xx++)
			{
				if (!TestEdges || ei.test(eqn))
				{
					pi.x = xx;
					pi.y = yy;
					Derived::drawPixel(pi);
				}

				pi.stepX(eqn, Derived::AVarCount, Derived::PVarCount, Derived::InterpolateZ, Derived::InterpolateW);
				if (TestEdges)
					ei.stepX(eqn);
			}

			po.stepY(eqn, Derived::AVarCount, Derived::PVarCount, Derived::InterpolateZ, Derived::InterpolateW);
			if (TestEdges)
				eo.stepY(eqn);
		}
	}

	static void drawSpan(const TriangleEquations &eqn, int x, int y, int x2)
	{
		float xf = x + 0.5f;
		float yf = y + 0.5f;

		PixelData p;
		p.y = y;
		p.init(eqn, xf, yf, Derived::AVarCount, Derived::PVarCount, Derived::InterpolateZ, Derived::InterpolateW);

		while (x < x2)
		{
			p.x = x;
			Derived::drawPixel(p);
			p.stepX(eqn, Derived::AVarCount, Derived::PVarCount, Derived::InterpolateZ, Derived::InterpolateW);
			x++;
		}
	}

	/// This is called per pixel. 
	/** Implement this in your derived class to display single pixels. */
	static void drawPixel(const PixelData &p)
	{

	}

protected:
	static PixelData copyPixelData(PixelData &po)
	{
		PixelData pi;
		if (Derived::InterpolateZ) pi.z = po.z;
		if (Derived::InterpolateW) pi.invw = po.invw;
		for (int i = 0; i < Derived::AVarCount; ++i)
			pi.avar[i] = po.avar[i];
		return pi;
	}
};

class DummyPixelShader : public PixelShaderBase<DummyPixelShader> {};

} // end namespace swr