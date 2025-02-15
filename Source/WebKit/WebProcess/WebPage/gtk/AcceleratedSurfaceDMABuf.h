/*
 * Copyright (C) 2023 Igalia S.L.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "AcceleratedSurface.h"

#if USE(GBM)
#include "MessageReceiver.h"
#include <WebCore/PageIdentifier.h>
#include <wtf/unix/UnixFileDescriptor.h>

typedef void *EGLImage;
struct gbm_bo;

namespace WebKit {

class ShareableBitmap;
class ShareableBitmapHandle;
class WebPage;

class AcceleratedSurfaceDMABuf final : public AcceleratedSurface, public IPC::MessageReceiver {
public:
    static std::unique_ptr<AcceleratedSurfaceDMABuf> create(WebPage&, Client&);
    ~AcceleratedSurfaceDMABuf();

    uint64_t window() const override { return 0; }
    uint64_t surfaceID() const override;
    void clientResize(const WebCore::IntSize&) override;
    bool shouldPaintMirrored() const override
    {
#if PLATFORM(GTK) && USE(GTK4)
        return false;
#else
        return true;
#endif
    }
    void didCreateGLContext() override;
    void willDestroyGLContext() override;
    void willRenderFrame() override;
    void didRenderFrame() override;

private:
    AcceleratedSurfaceDMABuf(WebPage&, Client&);

    // IPC::MessageReceiver.
    void didReceiveMessage(IPC::Connection&, IPC::Decoder&) override;

    void frameDone();

    class RenderTarget {
        WTF_MAKE_FAST_ALLOCATED;
    public:
        virtual ~RenderTarget();

        void willRenderFrame() const;
        virtual void didRenderFrame() const { }
        virtual void swap();

    protected:
        explicit RenderTarget(WebCore::PageIdentifier, const WebCore::IntSize&);

        WebCore::PageIdentifier m_pageID;
        unsigned m_backColorBuffer { 0 };
        unsigned m_frontColorBuffer { 0 };
        unsigned m_depthStencilBuffer { 0 };
    };

    class RenderTargetEGLImage final : public RenderTarget {
    public:
        static std::unique_ptr<RenderTarget> create(WebCore::PageIdentifier, const WebCore::IntSize&);
        RenderTargetEGLImage(WebCore::PageIdentifier, const WebCore::IntSize&, EGLImage, EGLImage, WTF::UnixFileDescriptor&&, WTF::UnixFileDescriptor&&, uint32_t format, uint32_t offset, uint32_t stride, uint64_t modifier);
        ~RenderTargetEGLImage();

    private:
        void swap() override;

        EGLImage m_backImage { nullptr };
        EGLImage m_frontImage { nullptr };
    };

    class RenderTargetSHMImage final : public RenderTarget {
    public:
        static std::unique_ptr<RenderTarget> create(WebCore::PageIdentifier, const WebCore::IntSize&);
        RenderTargetSHMImage(WebCore::PageIdentifier, const WebCore::IntSize&, Ref<ShareableBitmap>&&, ShareableBitmapHandle&&, Ref<ShareableBitmap>&&, ShareableBitmapHandle&&);
        ~RenderTargetSHMImage() = default;

    private:
        void didRenderFrame() const override;
        void swap() override;

        Ref<ShareableBitmap> m_backBitmap;
        Ref<ShareableBitmap> m_frontBitmap;
    };

    bool m_isSoftwareRast { false };
    unsigned m_fbo { 0 };
    std::unique_ptr<RenderTarget> m_target;
};

} // namespace WebKit

#endif // USE(GBM)
