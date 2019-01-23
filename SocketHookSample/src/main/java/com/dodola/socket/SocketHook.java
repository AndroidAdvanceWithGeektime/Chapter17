package com.dodola.socket;

public final class SocketHook {
    static {
        System.loadLibrary("sockethook");
    }

    private static boolean sHasHook = false;
    private static boolean sHookFailed = false;


    public static String getStack() {
        return stackTraceToString(new Throwable().getStackTrace());
    }

    private static String stackTraceToString(final StackTraceElement[] arr) {
        if (arr == null) {
            return "";
        }

        StringBuffer sb = new StringBuffer();

        for (StackTraceElement stackTraceElement : arr) {
            String className = stackTraceElement.getClassName();
            // remove unused stacks
            if (className.contains("java.lang.Thread")) {
                continue;
            }

            sb.append(stackTraceElement).append('\n');
        }
        return sb.toString();
    }

    public static void enableSocketHook() {
        if (sHasHook) {
            return;
        }
        sHasHook = true;
        enableSocketHookNative();

    }




    private static native void enableSocketHookNative();


}