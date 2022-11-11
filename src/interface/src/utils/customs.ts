// Copyright 2022 Marzuk Rashid

/**
 * This file serves as our Customs and Border Control between arbitrary objects
 * and our typed interfaces. The 'agents' in this file will look at an object
 * and 'stamp' it if it is a valid typescript object of the type they are
 * trained to inspect.
 * 
 * Agents can route objects to other agents. For example, the ArrayAgent will
 * first check that the object is a valid array, then it will route the items of
 * the array to whichever Agent(s) are handling the type of the items of the
 * array.
 */

import { mapKeys, camelCase, mapValues } from 'lodash';

/** */
/** Base and Utility Function and Types ***************************************/
/** */

abstract class BaseAgent<T = unknown> {
  /**
   * An object of the type that this Agent inspects for and approves unioned
   * with undefined so we don't have to define an actual value for it. We only
   * care about the type.
   */
  defaultValue: T | undefined;

  /** A function that checks if the given value is a T. */
  abstract verifyStamp(requested: unknown): requested is T;

  /**
   * A function that checks if the given value can be made into a T with
   * makeStampable.
   */
  canStamp(requested: unknown): boolean {
    const stampable = this.makeStampable(requested);
    return this.verifyStamp(stampable);
  };

  /**
   * If we need to make any modifications to requested before we can stamp it,
   * we do that here.
   */
  makeStampable(requested: unknown) {
    return requested;
  }

  /**
   * A function that takes some value we want to convert to `T`, verifies if the
   * conversion is valid, and returns the converted value if so. Otherwise, we
   * throw.
   */
  stamp(requested: unknown): T {
    const stampable = this.makeStampable(requested);
    if (this.verifyStamp(stampable)) return stampable;
    throw new Error(`${requested} cannot be stamped by agent ${this}`);
  }
}

/**
 * Takes the given Agent and gives back the type of values that agent inspects
 * and approves. */
export type Stamped
<T extends BaseAgent<unknown>> = Exclude<T['defaultValue'], undefined>;

export function hasProperty
<P extends PropertyKey>
(data: object, property: P): data is Record<P, unknown> {
  return property in data;
};

export type Nullable<T> = {
  [Property in keyof T]: T[Property] | null;
}

/** */
/** Primitives ****************************************************************/
/** */

class StringAgent extends BaseAgent<string> {
  verifyStamp(requested: unknown): requested is string {
    return typeof requested === 'string';
  }
}
export const stringAgent = new StringAgent();

class NumberAgent extends BaseAgent<number> {
  verifyStamp(requested: unknown): requested is number {
    return typeof requested === 'number';
  }
}
export const numberAgent = new NumberAgent();

class BooleanAgent extends BaseAgent<boolean> {
  verifyStamp(requested: unknown): requested is boolean {
    return typeof requested === 'boolean';
  }
}
export const booleanAgent = new BooleanAgent();

class NullAgent extends BaseAgent<null> {
  verifyStamp(requested: unknown): requested is null {
    return requested === null;
  }
}
export const nullAgent = new NullAgent();

/** */
/** Union Agent ***************************************************************/
/** */

class UnionAgent
<U extends BaseAgent<Stamped<U>>, V extends BaseAgent<Stamped<V>>>
extends BaseAgent<Stamped<U> | Stamped<V>> {
  directReports: {
    u: U,
    v: V,
  };
  constructor(uAgent: U, vAgent: V) {
    super();
    this.directReports = {
      u: uAgent,
      v: vAgent,
    };
  }
  makeStampable(requested: unknown): unknown {
    if (this.directReports.u.canStamp(requested)) {
      return this.directReports.u.makeStampable(requested);
    } else if (this.directReports.v.canStamp(requested)) {
      return this.directReports.v.makeStampable(requested);
    }
    return requested;
  }
  verifyStamp(requested: unknown): requested is Stamped<U> | Stamped<V> {
    return (
      this.directReports.u.verifyStamp(requested)
      || this.directReports.v.verifyStamp(requested)
    );
  }
}
export function unionAgent
<U extends BaseAgent<Stamped<U>>, V extends BaseAgent<Stamped<V>>>
(leftType: U, rightType: V) {
  return new UnionAgent(leftType, rightType);
}
export function nullableAgent<T extends BaseAgent<Stamped<T>>>(otherAgent: T) {
  return unionAgent(nullAgent, otherAgent);
}

/** */
/** Array Agent ***************************************************************/
/** */

class ArrayAgent
<T extends BaseAgent<Stamped<T>>>
extends BaseAgent<Array<Stamped<T>>> {
  itemAgent: T
  constructor(itemAgent: T) {
    super();
    this.itemAgent = itemAgent;
  }
  makeStampable(requested: unknown): unknown {
    if (!Array.isArray(requested)) return requested;
    return requested.map((value) => this.itemAgent.makeStampable(value));
  }
  verifyStamp(requested: unknown): requested is Array<Stamped<T>> {
    if (!Array.isArray(requested)) return false;
    return requested.every(v => this.itemAgent.verifyStamp(v));
  }
}
export const arrayAgent = <T extends BaseAgent<Stamped<T>>>(itemAgent: T) => (
  new ArrayAgent(itemAgent)
);

/** */
/** Enum Agent ****************************************************************/
/** */

type EnumValue<E> =
  E extends Record<string, infer V>
    ? Exclude<V, string> extends never
      ? V
      : Exclude<V, string>
    : never;
type Enum<E> = Record<keyof E, EnumValue<E>>;
class EnumAgent
<E extends Enum<E>>
extends BaseAgent<EnumValue<E>> {
  enumObj: E;
  constructor(enumObj: E) {
    super();
    this.enumObj = enumObj;
  }
  verifyStamp(requested: unknown): requested is EnumValue<E> {
    const enumValues = Object.values(this.enumObj);
    return enumValues.includes(requested);
  }
}
export const enumAgent = <T extends Enum<T>>(enumObj: T) => (
  new EnumAgent(enumObj)
);

/** */
/** Object Agent **************************************************************/
/** */

type StampedObject<O extends Record<string, BaseAgent>> = {
  [Property in keyof O]: Stamped<O[Property]>
}
class ObjectAgent
<O extends Record<string, BaseAgent>>
extends BaseAgent<StampedObject<O>> {
  objShape: O;
  constructor(objShape: O) {
    super();
    this.objShape = objShape;
  }
  makeStampable(requested: unknown): unknown {
    if (typeof requested !== 'object' || requested === null) return false;
    return mapValues(requested, (value, key) => {
      if (this.objShape[key] === undefined) return value;
      return this.objShape[key].makeStampable(value);
    });
  }
  verifyStamp(requested: unknown): requested is StampedObject<O> {
    if (typeof requested !== 'object' || requested === null) return false;
    return Object.entries(this.objShape).every(([property, agent]) => {
      if (!hasProperty(requested, property)) return false;
      return agent.verifyStamp(requested[property]);
    });
  }
}
export function objectAgent
<O extends Record<string, BaseAgent>>(shapeObj: O) {
  return new ObjectAgent(shapeObj);
}

/** */
/** Unsnake Agent *************************************************************/
/** */

class UnsnakeAgent
<O extends Record<string, BaseAgent>>
extends BaseAgent<StampedObject<O>> {
  objAgent: ObjectAgent<O>;
  constructor(objAgent: ObjectAgent<O>) {
    super();
    this.objAgent = objAgent;
  }
  verifyStamp(requested: unknown): requested is StampedObject<O> {
    return this.objAgent.verifyStamp(requested);
  }
  makeStampable(requested: unknown): unknown {
    if (typeof requested !== 'object' || requested === null) return requested;
    const unsnaked = mapKeys(requested, (_, key) => camelCase(key));
    return this.objAgent.makeStampable(unsnaked);
  }
}
export const unsnakeAgent = <O extends Record<string, BaseAgent>>(
  objAgent: ObjectAgent<O>
) => new UnsnakeAgent(objAgent);

/** */
/** Conversion Agent **********************************************************/
/** */

class ConversionAgent
<I extends BaseAgent<Stamped<I>>, O extends BaseAgent<Stamped<O>>>
extends BaseAgent<Stamped<O>> {
  inputAgent: I;
  converter: (arg: Stamped<I>) => Stamped<O>;
  outputAgent: O;
  constructor(
    inputAgent: I, converter: (arg: Stamped<I>) => Stamped<O>, outputAgent: O
  ) {
    super();
    this.inputAgent = inputAgent;
    this.converter = converter;
    this.outputAgent = outputAgent;
  }
  verifyStamp(requested: unknown): requested is Stamped<O> {
    return this.outputAgent.verifyStamp(requested);
  }
  makeStampable(requested: unknown): unknown {
    if (!this.inputAgent.verifyStamp(requested)) return requested
    return this.converter(requested);
  }
  stamp(requested: unknown) {
    if (this.inputAgent.verifyStamp(requested)) {
      return this.converter(requested);
    }
    throw new Error(`${requested} cannot be stamped by agent ${this}`);
  }
}
export function conversionAgent
<I extends BaseAgent<Stamped<I>>, O extends BaseAgent<Stamped<O>>>(
  inputAgent: I, converter: (arg: Stamped<I>) => Stamped<O>, outputAgent: O
) {
  return new ConversionAgent(inputAgent, converter, outputAgent);
}
