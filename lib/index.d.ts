export interface ApexOptions {
  [key: string]: unknown;
}

export declare function convert(markdown: string, options?: ApexOptions): string;
export declare const version: string;
